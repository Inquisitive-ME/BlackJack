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

class BJGame {
private:
    std::vector<abstractGamePlayer *> PlayersInGame;
    deck gDeck;
    dealer Dealer;
    bool PRINT_OUTPUT = true;

public:
    BJGame();
    void startGame(std::vector<abstractGamePlayer *> players);
    void starting_deal();
    void printHand(std::string name, BJHand& pHand);
    deck getDeck(){return gDeck;};
    dealer getDealer(){return Dealer;};
    void printDeck();
    virtual ~BJGame(){};
};

#endif //BLACKJACK_BJGAME_H
