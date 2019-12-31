//
// Created by richard on 12/23/19.
//

#include "BJGame.h"

using namespace std;

namespace BJGameFunctions
{
    void printHand(std::string name,BJHand& pHand)
    {
        std::cout << name << " your hand is ";
        pHand.print();
    }

    void starting_deal(deck &gameDeck, dealer &gameDealer, vector<abstractGamePlayer *> PlayersInGame)
    {
        //TODO this is where you would get the prebet Deck
        for_each(PlayersInGame.begin(), PlayersInGame.end(), [&gameDeck](abstractGamePlayer *player){
            player->newHand(player->getBet());
            gameDeck.deal(*player, 2);
        });

        gameDeck.deal(gameDealer,2);
        gameDealer.flipCard(0);
    }

    // TODO complete evaluate dealer has 21, consider having logic in player class, or adding callbacks for abstract class
    void evaluate_dealer_has_blackjack(vector<abstractGamePlayer*> PlayersInGame, deck &gDeck, bool PRINT_OUTPUT)
    {
        for(abstractGamePlayer *player : PlayersInGame)
        {
            for(BJHand pHand : player->getHands())
            {
                // TODO this could be a function that get's passed into for each
                if(PRINT_OUTPUT){printHand(player->getName(), pHand);}
                while(not(pHand.isBusted()) && pHand.getTotal() <21)
                {
                    gDeck.deal(pHand,1);
                    if(PRINT_OUTPUT){printHand(player->getName(), pHand);}
                }
                if(pHand.isBusted())
                {
                    if(PRINT_OUTPUT){cout << player->getName() << " BUSTED " << endl;}
                    player->loseHand(0);
                }
                else
                {
                    if(PRINT_OUTPUT){cout << player->getName() << " PUSHED " << std::endl;}
                    //player->push();
                }
            }
        }
    }
}

BJGame::BJGame() {
    gDeck.populate();
}

void BJGame::startGame(std::vector<abstractGamePlayer*> players)
{
    PlayersInGame = players;
}

void BJGame::printDeck()
{
    cout << "Card Remaining in Deck " << gDeck.getNumCards() << endl;
    cout << "Count: " << gDeck.getCount() << endl;
}