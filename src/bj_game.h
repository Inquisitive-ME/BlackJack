//
// Created by richard on 12/23/19.
//

#ifndef BLACKJACK_BJGAME_H
#define BLACKJACK_BJGAME_H

#include <vector>
#include "card.h"
#include "bj_hand.h"
#include "deck.h"
#include "dealer.h"
#include "player_interface.h"
#include "ai_interface.h"

namespace BJGameFunctions {
    using std::vector;

    void printHand(std::string name, BJHand &pHand);

    void deal_to_all_players(deck &gameDeck, vector<PlayerInterface *> PlayersInGame, AiInterface &gameAI);

    void deal_to_dealer(deck &gameDeck, dealer &gameDealer);

    void evaluate_dealer_has_blackjack(vector<PlayerInterface *> PlayersInGame, deck &gDeck, bool PRINT_OUTPUT = false);
}

enum GameState {
    BET,
    DEAL,
    INSURANCE,
    MOVE,
    RESULT,
    GAME_OVER
};

class BJGame {
private:
    std::vector<PlayerInterface *> gamePlayers;
    AiInterface &gameAI;
    deck gameDeck;
    dealer gameDealer;
    GameState gameState;

public:
    BJGame(std::vector<PlayerInterface *> players, AiInterface &ai);

    virtual ~BJGame() {};

    void printDeck();

    GameState deal();

    void play();
};

#endif //BLACKJACK_BJGAME_H
