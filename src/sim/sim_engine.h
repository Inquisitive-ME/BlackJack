#ifndef BJ_SIM_ENGINE_H
#define BJ_SIM_ENGINE_H

#include "sim/cards.h"
#include "sim/dealer.h"
#include "sim/hand.h"
#include "sim/round.h"
#include "sim/rules.h"
#include "sim/shoe.h"

#include <cstdint>
#include <vector>

namespace bj {

// Discrete bet spread (units). Chosen at the start of each round; the geometric
// spread is the realistic card-counting bet range.
inline constexpr int BET_LEVELS = 5;
inline constexpr double BET_UNITS[BET_LEVELS] = {1.0, 2.0, 4.0, 8.0, 16.0};

// Action space: indices 0..4 pick a bet level (bet phase); indices 5..9 are the
// play moves Hit/Stand/Double/Split/Surrender (play phase). The legal-action
// mask in the observation says which are valid.
inline constexpr int A_HIT = 5, A_STAND = 6, A_DOUBLE = 7, A_SPLIT = 8, A_SURRENDER = 9;
inline constexpr int NUM_ACTIONS = 10;

enum class Phase { Bet, Play, Done };

// Observation layout (float, numpy/torch friendly).
inline constexpr int OBS_SIZE = 35;
inline constexpr int O_COMP = 0;      // [0..9]  used fraction per rank bucket (deck history)
inline constexpr int O_PEN = 10;      // penetration
inline constexpr int O_TOTAL = 11;    // player total / 21
inline constexpr int O_SOFT = 12;     // soft flag
inline constexpr int O_PAIR = 13;     // pair flag
inline constexpr int O_UP = 14;       // [14..23] dealer up-card one-hot
inline constexpr int O_MASK = 24;     // [24..33] legal-action mask over the 10 actions
inline constexpr int O_PHASE = 34;    // 0 = bet, 1 = play

struct Observation {
    float x[OBS_SIZE];
};

// A Gymnasium-style blackjack environment: reset(seed) -> observation, and
// step(action) -> (observation, reward, done). An episode is one shoe, so the
// deck composition (the count) evolves across rounds within an episode. No I/O.
//
// Reuses the pure helpers (naturals/peek, settleHand, dealerPlay) that the
// reference playRound() is built from; a golden test checks the two agree.
class SimEngine {
public:
    explicit SimEngine(const RulesConfig &rules = {})
        : rules_(rules), shoe_(rules.decks, rules.penetration, 0) {}

    struct StepResult {
        Observation obs;
        double reward;
        bool done;
    };

    Observation reset(std::uint64_t seed) {
        shoe_.reset(seed);
        hands_.clear();
        cur_ = 0;
        splits_ = 0;
        rounds_ = 0;
        phase_ = Phase::Bet;
        return observation();
    }

    StepResult step(int action) {
        if (phase_ == Phase::Done) return {observation(), 0.0, true};
        if (phase_ == Phase::Bet) return stepBet(action);
        return stepPlay(action);
    }

    // --- read-only info channel (for adapters / tests / debugging) ---
    Phase phase() const { return phase_; }
    int rounds() const { return rounds_; }
    Card dealerUp() const { return up_; }
    const Hand &currentHand() const { return hands_[cur_].h; }
    const Shoe<> &shoe() const { return shoe_; }
    void legality(bool &canDouble, bool &canSplit, bool &canSurrender) const {
        computeLegality(canDouble, canSplit, canSurrender);
    }
    static int playActionIndex(Action a) {
        switch (a) {
            case Action::Hit: return A_HIT;
            case Action::Stand: return A_STAND;
            case Action::Double: return A_DOUBLE;
            case Action::Split: return A_SPLIT;
            case Action::Surrender: return A_SURRENDER;
        }
        return A_STAND;
    }

private:
    struct PHand {
        Hand h;
        double bet;
        bool splitAce;
        bool surrendered;
        bool done;
    };

    RulesConfig rules_;
    Shoe<> shoe_;
    Phase phase_ = Phase::Bet;
    std::vector<PHand> hands_;
    std::size_t cur_ = 0;
    Hand dealer_;
    Card up_ = 0;
    Card hole_ = 0;
    int splits_ = 0;
    int rounds_ = 0;

    static Action toPlayAction(int a) {
        switch (a) {
            case A_HIT: return Action::Hit;
            case A_STAND: return Action::Stand;
            case A_DOUBLE: return Action::Double;
            case A_SPLIT: return Action::Split;
            case A_SURRENDER: return Action::Surrender;
        }
        return Action::Stand;
    }

    void computeLegality(bool &canDouble, bool &canSplit, bool &canSurrender) const {
        const PHand &ph = hands_[cur_];
        bool firstDecision = (ph.h.numCards() == 2);
        bool original = (splits_ == 0);
        canSplit = firstDecision && ph.h.isPair() && splits_ < rules_.maxSplits &&
                   (ph.h.pairCard() != ACE || rules_.resplitAces || splits_ == 0);
        canDouble = firstDecision && (original || rules_.doubleAfterSplit);
        canSurrender = rules_.surrenderAllowed && original && firstDecision && hands_.size() == 1;
    }

    StepResult stepBet(int action) {
        double bet = BET_UNITS[(action >= 0 && action < BET_LEVELS) ? action : 0];
        ++rounds_;

        hands_.clear();
        cur_ = 0;
        splits_ = 0;
        Hand player;
        Card p1 = shoe_.dealFaceUp();
        up_ = shoe_.dealFaceUp();
        Card p2 = shoe_.dealFaceUp();
        hole_ = shoe_.dealHole();
        player.add(p1);
        player.add(p2);
        dealer_ = Hand{};
        dealer_.add(up_);
        hands_.push_back(PHand{player, bet, false, false, false});

        bool playerBJ = player.isBlackjack();
        bool dealerBJ = false;
        if (up_ == TEN || up_ == ACE) {
            Hand peek = dealer_;
            peek.add(hole_);
            dealerBJ = peek.isBlackjack();
        }
        if (playerBJ || dealerBJ) {
            shoe_.reveal(hole_);
            double r = (playerBJ && dealerBJ) ? 0.0
                       : playerBJ             ? rules_.blackjackPays * bet
                                              : -bet;
            return finishRound(r);
        }
        phase_ = Phase::Play;
        return {observation(), 0.0, false};
    }

    StepResult stepPlay(int action) {
        applyPlay(action);
        while (cur_ < hands_.size() && hands_[cur_].done) ++cur_;
        if (cur_ >= hands_.size()) return finishRound(showdownAndSettle());
        return {observation(), 0.0, false};
    }

    void applyPlay(int action) {
        bool canDouble, canSplit, canSurrender;
        computeLegality(canDouble, canSplit, canSurrender);
        Action a = toPlayAction(action);
        PHand &ph = hands_[cur_];

        if (a == Action::Split && canSplit) {
            Card pc = ph.h.pairCard();
            bool ace = (pc == ACE);
            double bet = ph.bet;
            Hand h1;
            h1.add(pc);
            h1.add(shoe_.dealFaceUp());
            Hand h2;
            h2.add(pc);
            h2.add(shoe_.dealFaceUp());
            ph.h = h1;
            ph.splitAce = ace;
            ph.done = ace; // split aces: one card, stand
            hands_.insert(hands_.begin() + cur_ + 1, PHand{h2, bet, ace, false, ace});
            ++splits_;
        } else if (a == Action::Double && canDouble) {
            ph.bet *= 2.0;
            ph.h.add(shoe_.dealFaceUp());
            ph.done = true;
        } else if (a == Action::Surrender && canSurrender) {
            ph.surrendered = true;
            ph.done = true;
        } else if (a == Action::Hit) {
            ph.h.add(shoe_.dealFaceUp());
            if (ph.h.isBusted()) ph.done = true;
        } else {
            ph.done = true; // Stand, or a masked illegal action
        }
    }

    double showdownAndSettle() {
        shoe_.reveal(hole_);
        dealer_.add(hole_);
        bool anyLive = false;
        for (const PHand &ph : hands_)
            if (!ph.surrendered && !ph.h.isBusted()) anyLive = true;
        if (anyLive) dealerPlay(dealer_, shoe_, rules_.hitSoft17);

        double reward = 0.0;
        for (const PHand &ph : hands_)
            reward += ph.surrendered ? -0.5 * ph.bet : settleHand(ph.h, dealer_, ph.bet);
        return reward;
    }

    StepResult finishRound(double reward) {
        bool done = shoe_.needsShuffle();
        phase_ = done ? Phase::Done : Phase::Bet;
        return {observation(), reward, done};
    }

    Observation observation() const {
        Observation o{};
        for (int b = 0; b < NUM_BUCKETS; ++b)
            o.x[O_COMP + b] = shoe_.full(b) > 0
                                  ? float(shoe_.seen(b)) / float(shoe_.full(b))
                                  : 0.0f;
        o.x[O_PEN] = float(shoe_.cardsDealt()) / float(shoe_.size());

        if (phase_ == Phase::Play && cur_ < hands_.size()) {
            const Hand &h = hands_[cur_].h;
            o.x[O_TOTAL] = float(h.total()) / 21.0f;
            o.x[O_SOFT] = h.isSoft() ? 1.0f : 0.0f;
            o.x[O_PAIR] = h.isPair() ? 1.0f : 0.0f;
            o.x[O_UP + up_] = 1.0f;
        }

        if (phase_ == Phase::Bet) {
            for (int i = 0; i < BET_LEVELS; ++i) o.x[O_MASK + i] = 1.0f;
        } else if (phase_ == Phase::Play) {
            bool cd, cs, csu;
            computeLegality(cd, cs, csu);
            o.x[O_MASK + A_HIT] = 1.0f;
            o.x[O_MASK + A_STAND] = 1.0f;
            if (cd) o.x[O_MASK + A_DOUBLE] = 1.0f;
            if (cs) o.x[O_MASK + A_SPLIT] = 1.0f;
            if (csu) o.x[O_MASK + A_SURRENDER] = 1.0f;
        }
        o.x[O_PHASE] = (phase_ == Phase::Play) ? 1.0f : 0.0f;
        return o;
    }
};

} // namespace bj

#endif // BJ_SIM_ENGINE_H
