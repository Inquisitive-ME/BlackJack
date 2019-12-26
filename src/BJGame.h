//
// Created by richard on 12/23/19.
//

#ifndef BLACKJACK_BJGAME_H
#define BLACKJACK_BJGAME_H

#include <vector>
#include "card.h"
#include "BJHand.h"
#include "deck.h"
#include "abstractGamePlayer.h"
#include "dealer.h"

namespace BJGameFunctions {
    using std::vector;
    void printHand(std::string name, BJHand& pHand);
    void starting_deal(deck &gameDeck, dealer &gameDealer, vector<abstractGamePlayer*> PlayersInGame);
    void evaluate_dealer_has_blackjack(vector<abstractGamePlayer*> PlayersInGame, deck &gDeck, bool PRINT_OUTPUT = false);
}

class BJGame {
private:
    std::vector<abstractGamePlayer *> PlayersInGame;
    deck gDeck;
    dealer Dealer;
    bool PRINT_OUTPUT = true;

public:
    BJGame();
    void startGame(std::vector<abstractGamePlayer *> players);
    void printDeck();
    virtual ~BJGame(){};
};

#endif //BLACKJACK_BJGAME_H
