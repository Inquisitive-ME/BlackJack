//
// Created by richard on 12/23/19.
//

#include "bj_game.h"

using namespace std;

namespace BJGameFunctions {

    void printHand(std::string name, BJHand &pHand) {
        std::cout << name << " your hand is ";
        pHand.print();
    }

    void deal_to_all_players(deck &gameDeck, vector<PlayerInterface *> PlayersInGame, AiInterface &gameAI) {
        //TODO this is where you would get the prebet Deck
        for_each(PlayersInGame.begin(), PlayersInGame.end(), [&gameDeck, &gameAI](PlayerInterface *gamePlayer) {
            gamePlayer->newHand(gameAI.getPlayerBet(*gamePlayer));
            gameDeck.dealFaceUp(*gamePlayer, 2);
        });
    }

    void deal_to_dealer(deck &gameDeck, dealer &gameDealer) {
        gameDeck.dealFaceDown(gameDealer, 2);
        gameDealer.flipCard(0);
    }

    GameState deal_next_state(dealer &gameDealer){
        if(gameDealer.getCard(0).isAce()) {
            return INSURANCE;
        } else{
            return MOVE;
        }
    }

    // TODO complete evaluate dealer has 21, consider having logic in PlayerInterface class, or adding callbacks for abstract class
    void evaluate_dealer_has_blackjack(vector<PlayerInterface *> PlayersInGame, deck &gDeck, bool PRINT_OUTPUT) {
        for (PlayerInterface *player : PlayersInGame) {
            for (BJHand pHand : player->getHands()) {
                // TODO this could be a function that get's passed into for each
                if (PRINT_OUTPUT) { printHand(player->getName(), pHand); }
                while (not(pHand.isBusted()) && pHand.getTotal() < 21) {
                    gDeck.dealFaceDown(pHand, 1);
                    if (PRINT_OUTPUT) { printHand(player->getName(), pHand); }
                }
                if (pHand.isBusted()) {
                    if (PRINT_OUTPUT) { cout << player->getName() << " BUSTED " << endl; }
                    player->loseHand(0);
                } else {
                    if (PRINT_OUTPUT) { cout << player->getName() << " PUSHED " << std::endl; }
                    //PlayerInterface->push();
                }
            }
        }
    }
}

// TODO Currently planning on just having 1 "AI" that will handle all decisions for players

BJGame::BJGame(std::vector<PlayerInterface *> players, AiInterface &ai) : gamePlayers{std::move(players)}, gameAI{ai} {
    gameDeck.populate();
    gameState = DEAL;
}

void BJGame::printDeck() {
    cout << "Card Remaining in Deck " << gameDeck.getNumCards() << endl;
    cout << "Count: " << gameDeck.getCount() << endl;
}

void BJGame::play() {
    while (gameState != GAME_OVER) {
        switch (gameState) {
            case DEAL: {
                // TODO this is where you could get prebet deck
                BJGameFunctions::deal_to_all_players(gameDeck, gamePlayers, gameAI);
                BJGameFunctions::deal_to_dealer(gameDeck, gameDealer);
                gameState = BJGameFunctions::deal_next_state(gameDealer);
            }
            break;

            case INSURANCE: {
                for(PlayerInterface* player : gamePlayers)
                {
                    if(gameAI.payInsurance(*player)){
                        player->payInsurance();
                    }
                }
                gameState = MOVE;
            }
            break;
            case MOVE: {
                for (PlayerInterface *player : gamePlayers) {
                    uint numHands = player->numHands();
                    for(uint handNumber = 0; handNumber < numHands; handNumber++){
                        BJHand playerHand = player->getHand(handNumber);
                        MOVES playerMove = HIT;
                        bool canDoubleDown = true;
                        while(playerMove != STAY and playerMove != SURRENDER and !playerHand.isBusted()){
                            playerMove = gameAI.getMove(gameDealer.getFaceUpCards(), playerHand);
                            if(playerMove == HIT) {
                                gameDeck.dealFaceUp(*player, 1);
                            } else if(playerMove == SPLIT){
                                if(player->getHand(handNumber).getNumCards() == 2){
                                    player->split(handNumber);
                                    numHands = player->numHands();
                                } else {
                                    if(!gameAI.illegalMove()){break;}
                                }

                            } else if(playerMove == DOUBLED){
                                if(canDoubleDown) {
                                    player->doubleDown(handNumber);
                                    gameDeck.dealFaceUp(*player, handNumber);
                                } else{
                                    if(!gameAI.illegalMove()){break;}
                                }
                            } else if(playerMove == SURRENDER){
                                player->surrender(handNumber);
                            }
                            canDoubleDown = false;
                            playerHand = player->getHand(handNumber);
                        }
                    }
                }
                gameState = RESULT;
            }
            break;
            case RESULT: {
                gameDealer.flipCard(1);
                for (PlayerInterface *player : gamePlayers) {
                    uint numHands = player->numHands();
                    for(uint handNumber = 0; handNumber < numHands; handNumber++){
                        BJHand playerHand = player->getHand(handNumber);
                        if(playerHand.isBusted()){
                            player->loseHand(handNumber);
                        } else{
                            if(player->getHand(handNumber).getTotal() > gameDealer.getTotal()){
                                while(gameDealer.isHitting()){
                                    gameDeck.dealFaceUp(gameDealer, 1);
                                }
                            }
                            if(player->getHand(handNumber).getTotal() > gameDealer.getTotal()){
                                player->winHand(handNumber);
                            } else if(player->getHand(handNumber).getTotal() == gameDealer.getTotal()){
                                player->pushHand(handNumber);
                            } else{
                                player->loseHand(handNumber);
                            }
                        }
                    }
                    player->clearAllHands();
                    gameDealer.clear();
                }
                if(gameAI.continuePlaying()){
                    gameState = DEAL;
                } else{
                    std::cout<< "END" << std::endl;
                    gameState = GAME_OVER;
                }
            }
            break;
        }
    }
}