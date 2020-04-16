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

    void deal_to_all_players(deck &gameDeck, vector<unique_ptr<PlayerInterface>>& PlayersInGame, AiInterface &gameAI) {
        //TODO this is where you would get the prebet Deck
        for_each(PlayersInGame.begin(), PlayersInGame.end(), [&gameDeck, &gameAI](unique_ptr<PlayerInterface>& gamePlayer) {
            gamePlayer->newHand(gameAI.getPlayerBet(gamePlayer));
            gameDeck.dealFaceUp(gamePlayer, 2);
        });
    }

    void deal_to_dealer(deck &gameDeck, DealerInterface &gameDealer) {
        gameDeck.dealFaceDown(gameDealer, 2);
        gameDealer.flipCard(0);
    }

    GameState deal_next_state(DealerInterface &gameDealer){
        if(gameDealer.getCard(0).isAce()) {
            return INSURANCE;
        } else{
            return MOVE;
        }
    }

    // TODO complete evaluate dealer has 21, consider having logic in PlayerInterface class, or adding callbacks for abstract class
    void evaluate_dealer_has_blackjack(vector<unique_ptr<PlayerInterface>> PlayersInGame, deck &gDeck, bool PRINT_OUTPUT) {
        for (unique_ptr<PlayerInterface>& player : PlayersInGame) {
            for (uint handNumber = 0; handNumber < player->getNumHands(); handNumber++) {
                // TODO this could be a function that get's passed into for each
                while (player->getHand(handNumber).getTotal() < 21) {
                    gDeck.hitPlayerHand(player, handNumber);
                }
                if (player->getHand(handNumber).isBusted()) {
                    player->loseHand(handNumber);
                } else {
                    if (PRINT_OUTPUT) { cout << player->getName() << " PUSHED " << std::endl; }
                    player->pushHand(handNumber);
                }
            }
        }
    }
}

// TODO Currently planning on just having 1 "AI" that will handle all decisions for players

BJGame::BJGame(DealerInterface &dealer, std::vector<std::unique_ptr<PlayerInterface>>& players, AiInterface &ai) :
                gameDealer{dealer}, gamePlayers(players), gameAI{ai} {
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
            case BET:{} break;
            case DEAL: {
                // TODO this is where you could get prebet deck
                BJGameFunctions::deal_to_all_players(gameDeck, gamePlayers, gameAI);
                BJGameFunctions::deal_to_dealer(gameDeck, gameDealer);
                gameState = BJGameFunctions::deal_next_state(gameDealer);
            }
            break;

            case INSURANCE: {
                for(unique_ptr<PlayerInterface>& player : gamePlayers)
                {
                    if(gameAI.payInsurance(*player)){
                        player->payInsurance();
                    }
                }
                gameState = MOVE;
            }
            break;
            case MOVE: {
                for (unique_ptr<PlayerInterface>& player : gamePlayers) {
                    uint numHands = player->getNumHands();
                    for(uint handNumber = 0; handNumber < numHands; handNumber++){
                        MOVES playerMove = HIT;
                        bool canDoubleDown = true;
                        while(playerMove != STAY and playerMove != SURRENDER and !player->getHand(handNumber).isBusted()){
                            playerMove = gameAI.getMove(gameDealer.getFaceUpCards(), player->getHand(handNumber));
                            if(playerMove == HIT) {
                                gameDeck.hitPlayerHand(player, handNumber);
                            } else if(playerMove == SPLIT){
                                if(player->getHand(0).getNumCards() == 2){
                                    player->split(handNumber);
                                    numHands = player->getNumHands();
                                } else {
                                    if(!gameAI.illegalMove()){break;}
                                }
                            } else if(playerMove == DOUBLED){
                                if(canDoubleDown) {
                                    player->doubleDown(handNumber);
                                    gameDeck.hitPlayerHand(player, handNumber);
                                } else{
                                    if(!gameAI.illegalMove()){break;}
                                }
                            } else if(playerMove == SURRENDER){
                                player->surrender(handNumber);
                            }
                            canDoubleDown = false;
                        }
                    }
                }
                gameState = RESULT;
            }
            break;
            case RESULT: {
                gameDealer.flipCard(1);
                for (unique_ptr<PlayerInterface>& player : gamePlayers) {
                    uint numHands = player->getNumHands();
                    for(uint handNumber = 0; handNumber < numHands; handNumber++){
                        BJHand playerHand = player->getHand(handNumber);
                        if(playerHand.isBusted()){
                            player->loseHand(handNumber);
                        } else{
                            while(gameDealer.isHitting()){
                                gameDeck.dealFaceUp(gameDealer, 1);
                            }
                            if(player->getHand(handNumber).getTotal() > gameDealer.getTotal() or gameDealer.isBusted()){
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
            case GAME_OVER: {std::cout << "GAMEOVER" << std::endl;}
            break;
        }
    }
}