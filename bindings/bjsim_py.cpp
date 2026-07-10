// pybind11 binding for the fast blackjack simulator.
//
// Exposes three things to Python:
//   * SimEngine     -- the Gymnasium-style env (reset/step/observation).
//   * CountingGame  -- a fast contextual-bandit interface for the bet-sizing
//                      problem: observe the composition, choose a bet, and C++
//                      plays the round with basic strategy.
//   * evaluate_linear_policy -- a fully-C++ evaluator that scores a linear
//                      "count weights + bet ramp" policy over millions of rounds,
//                      so High-Low and a learned count can be compared with tight
//                      confidence intervals at C++ speed.
#include "sim/basic_strategy.h"
#include "sim/cards.h"
#include "sim/count_play.h"
#include "sim/round.h"
#include "sim/rules.h"
#include "sim/shoe.h"
#include "sim/sim_engine.h"

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include <array>
#include <cmath>
#include <cstdint>
#include <vector>

namespace py = pybind11;
using namespace bj;

namespace {

py::array_t<float> obsToNumpy(const Observation &o) {
    return py::array_t<float>(OBS_SIZE, o.x);
}

// Bet from a linear count ramp, rounded to whole units and clamped to the spread.
inline double rampBet(double trueCount, double a, double b, double betMin, double betMax) {
    double bet = std::round(a + b * trueCount);
    if (bet < betMin) bet = betMin;
    if (bet > betMax) bet = betMax;
    return bet;
}

// Fast contextual-bandit view of the game for the bet-sizing RL problem.
struct CountingGame {
    RulesConfig rules;
    Shoe<8> shoe;

    explicit CountingGame(RulesConfig r, std::uint64_t seed)
        : rules(r), shoe(r.decks, r.penetration, seed) {}

    void reset(std::uint64_t seed) { shoe.reset(seed); }
    bool needs_shuffle() const { return shoe.needsShuffle(); }
    void shuffle() { shoe.shuffle(); }
    double decks_remaining() const { return shoe.decksRemaining(); }
    double penetration() const { return double(shoe.cardsDealt()) / shoe.size(); }
    int cards_remaining() const { return shoe.cardsRemaining(); }

    // Revealed-card counts per rank bucket (0=Ace .. 9=ten-group): the deck
    // history / composition the agent bets on.
    py::array_t<double> seen() const {
        std::array<double, NUM_BUCKETS> t{};
        for (int b = 0; b < NUM_BUCKETS; ++b) t[b] = shoe.seen(b);
        return py::array_t<double>(NUM_BUCKETS, t.data());
    }
    py::array_t<double> remaining() const {
        std::array<double, NUM_BUCKETS> t{};
        for (int b = 0; b < NUM_BUCKETS; ++b) t[b] = shoe.remaining(b);
        return py::array_t<double>(NUM_BUCKETS, t.data());
    }
    // Remaining fraction per rank -- the normalized composition feature vector.
    py::array_t<double> remaining_fraction() const {
        std::array<double, NUM_BUCKETS> t{};
        for (int b = 0; b < NUM_BUCKETS; ++b)
            t[b] = shoe.full(b) > 0 ? double(shoe.remaining(b)) / shoe.full(b) : 0.0;
        return py::array_t<double>(NUM_BUCKETS, t.data());
    }

    double running_count(const std::vector<double> &w) const {
        double rc = 0.0;
        for (int b = 0; b < NUM_BUCKETS; ++b) rc += shoe.seen(b) * w[b];
        return rc;
    }
    double true_count(const std::vector<double> &w) const {
        double d = decks_remaining();
        return d > 0.0 ? running_count(w) / d : 0.0;
    }

    // Place `bet` units and play one round with basic strategy; returns net chips.
    // (Reward is linear in the bet, so this is bet * the 1-unit round result.)
    double bet_and_play(double bet) { return bet * playRound(shoe, rules, basicStrategy); }
};

// Score a linear "count weights + ramp" betting policy over `rounds` rounds of
// basic-strategy play. Returns a dict of statistics. Runs entirely in C++.
py::dict evaluate_linear_policy(const RulesConfig &rules, const std::vector<double> &weights,
                                double ramp_a, double ramp_b, double bet_min, double bet_max,
                                long rounds, std::uint64_t seed) {
    std::array<double, NUM_BUCKETS> w{};
    for (int b = 0; b < NUM_BUCKETS; ++b) w[b] = (b < (int)weights.size()) ? weights[b] : 0.0;

    double sum = 0.0, sumsq = 0.0, wagered = 0.0;
    long n = 0;
    {
        py::gil_scoped_release release; // pure C++, no Python calls inside
        Shoe<8> shoe(rules.decks, rules.penetration, seed);
        for (long i = 0; i < rounds; ++i) {
            if (shoe.needsShuffle()) shoe.shuffle();
            double rc = 0.0, dr = shoe.decksRemaining();
            for (int b = 0; b < NUM_BUCKETS; ++b) rc += shoe.seen(b) * w[b];
            double tc = dr > 0.0 ? rc / dr : 0.0;
            double bet = rampBet(tc, ramp_a, ramp_b, bet_min, bet_max);
            double reward = bet * playRound(shoe, rules, basicStrategy);
            sum += reward;
            sumsq += reward * reward;
            wagered += bet;
            ++n;
        }
    }
    double mean = sum / n;
    double var = sumsq / n - mean * mean;
    py::dict d;
    d["ev_per_round"] = mean;                       // expected chips won per round
    d["stderr"] = std::sqrt(var / n);               // standard error of the mean
    d["ev_per_unit"] = sum / wagered;               // edge per unit wagered
    d["avg_bet"] = wagered / n;
    d["rounds"] = n;
    return d;
}

// Compare two linear betting policies on the SAME shoes (common random numbers).
// Each round's 1-unit result is shared, so the paired difference EV_a - EV_b has
// far lower variance than two independent estimates -- essential for detecting a
// small edge improvement over an already near-optimal count like High-Low.
py::dict evaluate_two_linear_policies(const RulesConfig &rules, const std::vector<double> &wa,
                                      double a_a, double a_b, const std::vector<double> &wb,
                                      double b_a, double b_b, double bet_min, double bet_max,
                                      long rounds, std::uint64_t seed) {
    std::array<double, NUM_BUCKETS> WA{}, WB{};
    for (int b = 0; b < NUM_BUCKETS; ++b) {
        WA[b] = (b < (int)wa.size()) ? wa[b] : 0.0;
        WB[b] = (b < (int)wb.size()) ? wb[b] : 0.0;
    }
    double sumA = 0.0, sumB = 0.0, sumD = 0.0, sumDsq = 0.0;
    long n = 0;
    {
        py::gil_scoped_release release;
        Shoe<8> shoe(rules.decks, rules.penetration, seed);
        for (long i = 0; i < rounds; ++i) {
            if (shoe.needsShuffle()) shoe.shuffle();
            double dr = shoe.decksRemaining(), rcA = 0.0, rcB = 0.0;
            for (int b = 0; b < NUM_BUCKETS; ++b) {
                rcA += shoe.seen(b) * WA[b];
                rcB += shoe.seen(b) * WB[b];
            }
            double tcA = dr > 0.0 ? rcA / dr : 0.0;
            double tcB = dr > 0.0 ? rcB / dr : 0.0;
            double betA = rampBet(tcA, a_a, a_b, bet_min, bet_max);
            double betB = rampBet(tcB, b_a, b_b, bet_min, bet_max);
            double base = playRound(shoe, rules, basicStrategy); // shared 1-unit outcome
            double ra = betA * base, rb = betB * base, d = ra - rb;
            sumA += ra;
            sumB += rb;
            sumD += d;
            sumDsq += d * d;
            ++n;
        }
    }
    double meanD = sumD / n;
    double varD = sumDsq / n - meanD * meanD;
    py::dict out;
    out["ev_a"] = sumA / n;
    out["ev_b"] = sumB / n;
    out["ev_diff"] = meanD;                    // EV_a - EV_b (paired)
    out["stderr_diff"] = std::sqrt(varD / n);  // low-variance thanks to shared shoes
    out["rounds"] = n;
    return out;
}

// Score a High-Low counter over many rounds with either basic-strategy play or
// count-aware play (index deviations + insurance). Same betting for both, so the
// EV gap is purely the value of using the deck composition for PLAY decisions.
py::dict evaluate_counting(const RulesConfig &rules, const std::vector<double> &weights,
                           double ramp_a, double ramp_b, double bet_min, double bet_max,
                           bool deviations, double insurance_tc, long rounds, std::uint64_t seed) {
    std::array<double, NUM_BUCKETS> w{};
    for (int b = 0; b < NUM_BUCKETS; ++b) w[b] = (b < (int)weights.size()) ? weights[b] : 0.0;

    double sum = 0.0, sumsq = 0.0, wagered = 0.0;
    long n = 0;
    {
        py::gil_scoped_release release;
        Shoe<8> shoe(rules.decks, rules.penetration, seed);
        for (long i = 0; i < rounds; ++i) {
            if (shoe.needsShuffle()) shoe.shuffle();
            double rc = 0.0, dr = shoe.decksRemaining();
            for (int b = 0; b < NUM_BUCKETS; ++b) rc += shoe.seen(b) * w[b];
            double tc = dr > 0.0 ? rc / dr : 0.0;
            double bet = rampBet(tc, ramp_a, ramp_b, bet_min, bet_max);
            double reward = playRoundCounting(shoe, rules, tc, deviations, insurance_tc, bet);
            sum += reward;
            sumsq += reward * reward;
            wagered += bet;
            ++n;
        }
    }
    double mean = sum / n;
    double var = sumsq / n - mean * mean;
    py::dict d;
    d["ev_per_round"] = mean;
    d["stderr"] = std::sqrt(var / n);
    d["ev_per_unit"] = sum / wagered;
    d["avg_bet"] = wagered / n;
    d["rounds"] = n;
    return d;
}

} // namespace

PYBIND11_MODULE(_bjsim, m) {
    m.doc() = "Fast blackjack simulator with a deck-history observation (C++ core).";

    py::class_<RulesConfig>(m, "RulesConfig")
        .def(py::init<>())
        .def_readwrite("decks", &RulesConfig::decks)
        .def_readwrite("penetration", &RulesConfig::penetration)
        .def_readwrite("hit_soft_17", &RulesConfig::hitSoft17)
        .def_readwrite("blackjack_pays", &RulesConfig::blackjackPays)
        .def_readwrite("double_after_split", &RulesConfig::doubleAfterSplit)
        .def_readwrite("max_splits", &RulesConfig::maxSplits)
        .def_readwrite("surrender_allowed", &RulesConfig::surrenderAllowed)
        .def_readwrite("resplit_aces", &RulesConfig::resplitAces);

    py::class_<SimEngine>(m, "SimEngine")
        .def(py::init<RulesConfig>(), py::arg("rules") = RulesConfig{})
        .def("reset", [](SimEngine &e, std::uint64_t seed) { return obsToNumpy(e.reset(seed)); },
             py::arg("seed"))
        .def("step",
             [](SimEngine &e, int action) {
                 auto r = e.step(action);
                 return py::make_tuple(obsToNumpy(r.obs), r.reward, r.done);
             },
             py::arg("action"))
        .def("is_bet_phase", [](const SimEngine &e) { return e.phase() == Phase::Bet; })
        .def("is_done", [](const SimEngine &e) { return e.phase() == Phase::Done; })
        .def("rounds", &SimEngine::rounds);

    py::class_<CountingGame>(m, "CountingGame")
        .def(py::init<RulesConfig, std::uint64_t>(), py::arg("rules") = RulesConfig{}, py::arg("seed") = 0)
        .def("reset", &CountingGame::reset, py::arg("seed"))
        .def("needs_shuffle", &CountingGame::needs_shuffle)
        .def("shuffle", &CountingGame::shuffle)
        .def("decks_remaining", &CountingGame::decks_remaining)
        .def("penetration", &CountingGame::penetration)
        .def("cards_remaining", &CountingGame::cards_remaining)
        .def("seen", &CountingGame::seen)
        .def("remaining", &CountingGame::remaining)
        .def("remaining_fraction", &CountingGame::remaining_fraction)
        .def("running_count", &CountingGame::running_count, py::arg("weights"))
        .def("true_count", &CountingGame::true_count, py::arg("weights"))
        .def("bet_and_play", &CountingGame::bet_and_play, py::arg("bet"));

    m.def("evaluate_linear_policy", &evaluate_linear_policy, py::arg("rules"), py::arg("weights"),
          py::arg("ramp_a"), py::arg("ramp_b"), py::arg("bet_min"), py::arg("bet_max"),
          py::arg("rounds"), py::arg("seed"),
          "Score a linear count-weights + bet-ramp policy over many rounds (C++ speed).");

    m.def("evaluate_two_linear_policies", &evaluate_two_linear_policies, py::arg("rules"),
          py::arg("weights_a"), py::arg("ramp_a_a"), py::arg("ramp_a_b"), py::arg("weights_b"),
          py::arg("ramp_b_a"), py::arg("ramp_b_b"), py::arg("bet_min"), py::arg("bet_max"),
          py::arg("rounds"), py::arg("seed"),
          "Paired comparison of two linear betting policies on shared shoes (low variance).");

    m.def("evaluate_counting", &evaluate_counting, py::arg("rules"), py::arg("weights"),
          py::arg("ramp_a"), py::arg("ramp_b"), py::arg("bet_min"), py::arg("bet_max"),
          py::arg("deviations"), py::arg("insurance_tc"), py::arg("rounds"), py::arg("seed"),
          "Score a High-Low counter with basic vs count-aware play (deviations + insurance).");

    // Constants and reference tables.
    m.attr("OBS_SIZE") = OBS_SIZE;
    m.attr("NUM_ACTIONS") = NUM_ACTIONS;
    m.attr("BET_LEVELS") = BET_LEVELS;
    m.attr("O_MASK") = O_MASK;
    m.attr("O_PHASE") = O_PHASE;
    m.attr("A_HIT") = A_HIT;
    m.attr("A_STAND") = A_STAND;
    m.attr("A_DOUBLE") = A_DOUBLE;
    m.attr("A_SPLIT") = A_SPLIT;
    m.attr("A_SURRENDER") = A_SURRENDER;
    {
        std::array<double, NUM_BUCKETS> hilo{};
        for (int b = 0; b < NUM_BUCKETS; ++b) hilo[b] = W_HILO[b];
        m.attr("W_HILO") = py::array_t<double>(NUM_BUCKETS, hilo.data());
    }
}
