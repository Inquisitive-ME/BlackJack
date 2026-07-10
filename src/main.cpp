// Console blackjack -- a thin human front-end over the fast simulation core.
// The engine folds 10/J/Q/K into one bucket, so every ten-valued card prints as
// "10" (no suits). Ctrl-D quits.
#include "sim/cards.h"
#include "sim/dealer.h"
#include "sim/hand.h"
#include "sim/round.h"
#include "sim/rules.h"
#include "sim/shoe.h"

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace bj;

namespace {

const char *NAME[NUM_BUCKETS] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10"};

// A hand plus the concrete cards, kept only so we can print them.
struct HumanHand {
    Hand hand;
    std::vector<Card> cards;
    double bet = 1.0;
    bool splitAce = false;
    bool surrendered = false;
    bool done = false;
    void add(Card c) {
        hand.add(c);
        cards.push_back(c);
    }
};

std::string cardsStr(const std::vector<Card> &cs) {
    std::string s;
    for (Card c : cs) {
        s += NAME[c];
        s += ' ';
    }
    return s;
}

void showHand(const char *who, const HumanHand &h) {
    std::cout << who << ": " << cardsStr(h.cards) << "(total " << h.hand.total()
              << (h.hand.isSoft() ? ", soft" : "") << ")\n";
}

// Read a line; return the first char that is in `valid`, else re-prompt. EOF quits.
char askChar(const std::string &prompt, const std::string &valid) {
    for (;;) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nBye.\n";
            std::exit(0);
        }
        for (char ch : line) {
            if (!std::isspace((unsigned char)ch)) {
                char c = (char)std::tolower((unsigned char)ch);
                if (valid.find(c) != std::string::npos) return c;
                break;
            }
        }
        std::cout << "  enter one of [" << valid << "]\n";
    }
}

int askInt(const std::string &prompt, int lo, int hi) {
    for (;;) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cout << "\nBye.\n";
            std::exit(0);
        }
        try {
            int v = std::stoi(line);
            if (v >= lo && v <= hi) return v;
        } catch (...) {
        }
        std::cout << "  enter a number between " << lo << " and " << hi << "\n";
    }
}

} // namespace

int main() {
    RulesConfig rules;
    rules.surrenderAllowed = true; // let the human try every action
    std::random_device rd;
    Shoe<8> shoe(rules.decks, rules.penetration, rd());
    double purse = 0.0;

    std::cout << "=== Blackjack: " << rules.decks << " decks, "
              << (rules.hitSoft17 ? "dealer hits soft 17" : "dealer stands soft 17")
              << ", blackjack pays 3:2 ===\n"
              << "Ten-valued cards all print as \"10\". Ctrl-D to quit.\n";

    for (;;) {
        if (shoe.needsShuffle()) {
            shoe.shuffle();
            std::cout << "\n-- reshuffling the shoe --\n";
        }

        std::cout << "\nBankroll: " << purse << "   [cards left " << shoe.cardsRemaining()
                  << ", High-Low running count " << shoe.runningCount(W_HILO) << "]\n";
        int bet = askInt("Your bet (1-100): ", 1, 100);

        std::vector<HumanHand> hands(1);
        hands[0].bet = bet;
        hands[0].add(shoe.dealFaceUp());
        Card up = shoe.dealFaceUp();
        hands[0].add(shoe.dealFaceUp());
        Card hole = shoe.dealHole();
        Hand dealer;
        dealer.add(up);
        std::vector<Card> dealerCards = {up};

        std::cout << "Dealer shows: " << NAME[up] << "\n";
        showHand("You", hands[0]);

        bool playerBJ = hands[0].hand.isBlackjack();
        bool dealerBJ = false;
        if (up == TEN || up == ACE) {
            Hand peek = dealer;
            peek.add(hole);
            dealerBJ = peek.isBlackjack();
        }

        double insurance = 0.0;
        if (up == ACE && !playerBJ && askChar("Insurance? (y/n) ", "yn") == 'y') {
            insurance = bet / 2.0;
        }

        double result = 0.0;

        // Naturals end the round immediately.
        if (playerBJ || dealerBJ) {
            dealer.add(hole);
            dealerCards.push_back(hole);
            std::cout << "Dealer has: " << cardsStr(dealerCards) << "(total " << dealer.total() << ")\n";
            if (insurance > 0.0) result += dealerBJ ? 2.0 * insurance : -insurance;
            if (playerBJ && dealerBJ) {
                std::cout << "Both blackjack -- push.\n";
            } else if (playerBJ) {
                result += rules.blackjackPays * bet;
                std::cout << "Blackjack! You win " << rules.blackjackPays * bet << ".\n";
            } else {
                result += -bet;
                std::cout << "Dealer blackjack -- you lose.\n";
            }
            purse += result;
            continue;
        }

        // Player turn (splits append hands).
        int splits = 0;
        for (std::size_t i = 0; i < hands.size(); ++i) {
            if (hands.size() > 1) std::cout << "-- hand " << (i + 1) << " --\n";
            while (!hands[i].done) {
                HumanHand &h = hands[i];
                if (h.splitAce) { h.done = true; break; }
                if (h.hand.isBusted()) {
                    std::cout << "Busted.\n";
                    h.done = true;
                    break;
                }
                showHand("You", h);
                bool first = h.hand.numCards() == 2;
                bool canDouble = first;
                bool canSplit = first && h.hand.isPair() && splits < rules.maxSplits;
                bool canSurr = rules.surrenderAllowed && first && hands.size() == 1;
                std::string valid = "hs", menu = "(h)it (s)tand";
                if (canDouble) { valid += 'd'; menu += " (d)ouble"; }
                if (canSplit)  { valid += 'p'; menu += " s(p)lit"; }
                if (canSurr)   { valid += 'r'; menu += " su(r)render"; }
                char a = askChar(menu + "? ", valid);

                if (a == 's') {
                    h.done = true;
                } else if (a == 'h') {
                    Card c = shoe.dealFaceUp();
                    h.add(c);
                    std::cout << "  drew " << NAME[c] << "\n";
                } else if (a == 'd' && canDouble) {
                    h.bet *= 2.0;
                    Card c = shoe.dealFaceUp();
                    h.add(c);
                    std::cout << "  doubled, drew " << NAME[c] << "\n";
                    h.done = true;
                } else if (a == 'r' && canSurr) {
                    h.surrendered = true;
                    h.done = true;
                    std::cout << "  surrendered\n";
                } else if (a == 'p' && canSplit) {
                    Card pc = h.hand.pairCard();
                    bool ace = pc == ACE;
                    HumanHand nh;
                    nh.bet = h.bet;
                    h.hand = Hand{};
                    h.cards.clear();
                    h.add(pc);
                    h.add(shoe.dealFaceUp());
                    nh.add(pc);
                    nh.add(shoe.dealFaceUp());
                    h.splitAce = nh.splitAce = ace;
                    hands.insert(hands.begin() + i + 1, nh);
                    ++splits;
                }
            }
        }

        // Dealer turn.
        dealer.add(hole);
        dealerCards.push_back(hole);
        std::cout << "Dealer reveals: " << cardsStr(dealerCards) << "(total " << dealer.total() << ")\n";
        bool anyLive = false;
        for (const HumanHand &h : hands)
            if (!h.surrendered && !h.hand.isBusted()) anyLive = true;
        if (anyLive) {
            while (dealerShouldHit(dealer.total(), dealer.isSoft(), rules.hitSoft17)) {
                Card c = shoe.dealFaceUp();
                dealer.add(c);
                dealerCards.push_back(c);
                std::cout << "  dealer draws " << NAME[c] << " (total " << dealer.total() << ")\n";
            }
            if (dealer.isBusted()) std::cout << "Dealer busts!\n";
        }

        // Settle (dealer had no blackjack here, so any insurance is lost).
        if (insurance > 0.0) result -= insurance;
        for (std::size_t i = 0; i < hands.size(); ++i) {
            const HumanHand &h = hands[i];
            double r = h.surrendered ? -0.5 * h.bet : settleHand(h.hand, dealer, h.bet);
            result += r;
            std::cout << "  " << (hands.size() > 1 ? "hand " + std::to_string(i + 1) + ": " : "");
            if (h.surrendered) std::cout << "surrender (" << -0.5 * h.bet << ")\n";
            else if (r > 0) std::cout << "win +" << r << "\n";
            else if (r < 0) std::cout << "lose " << r << "\n";
            else std::cout << "push\n";
        }
        purse += result;
        std::cout << "Round result: " << (result >= 0 ? "+" : "") << result << "\n";
    }
}
