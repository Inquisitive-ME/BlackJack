//
// Created by richard on 12/23/19.
//

#ifndef BLACKJACK_BJGAME_H
#define BLACKJACK_BJGAME_H

#include <vector>
#include <memory>
#include "card.h"
#include "bj_hand.h"
#include "deck.h"
#include "dealer_interface.h"
#include "player_interface.h"
#include "ai_interface.h"

enum GameState {
    BET,
    DEAL,
    INSURANCE,
    MOVE,
    RESULT,
    GAME_OVER
};

namespace BJGameFunctions {
    using std::vector;

    void printHand(std::string name, BJHand &pHand);

    void deal_to_all_players(deck &gameDeck, vector<std::unique_ptr<PlayerInterface>>& PlayersInGame, AiInterface &gameAI);

    void deal_to_dealer(deck &gameDeck, DealerInterface &gameDealer);

    GameState deal_next_state(DealerInterface &gameDealer);

    void evaluate_dealer_has_blackjack(vector<std::unique_ptr<PlayerInterface>>& PlayersInGame, deck &gDeck, bool PRINT_OUTPUT = false);
}

class BJGame {
private:
    DealerInterface &gameDealer;
    std::vector<std::unique_ptr<PlayerInterface>> &gamePlayers;
    AiInterface &gameAI;
    deck gameDeck;
    GameState gameState;

public:
    BJGame(DealerInterface &dealer, std::vector<std::unique_ptr<PlayerInterface>> &players, AiInterface &ai);

    virtual ~BJGame() {};

    void printDeck();

    void play();
};

#endif //BLACKJACK_BJGAME_H
