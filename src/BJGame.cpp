//
// Created by richard on 12/23/19.
//

#include "BJGame.h"

using namespace std;
// TODO should I pass in deck fo testing?
BJGame::BJGame() {
    gDeck.populate();
}

void BJGame::startGame(std::vector<abstractGamePlayer*> players)
{
    PlayersInGame = players;
}

void BJGame::starting_deal()
{

    //TODO this is where you would get the prebet Deck
    //Get all players bets and deal hand
    for_each(PlayersInGame.begin(), PlayersInGame.end(), [this](abstractGamePlayer *player){
        player->newHand(player->getBet());
        gDeck.deal(*player, 2);
    });

    gDeck.deal(Dealer,2);
    Dealer.flipCard(0);
}

void BJGame::printDeck()
{
    cout << "Card Remaining in Deck " << gDeck.getNumCards() << endl;
    cout << "Count: " << gDeck.getCount() << endl;
}
