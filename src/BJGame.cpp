//
// Created by richard on 12/23/19.
//

#include "BJGame.h"

// TODO should I pass in deck fo testing?? Probably
BJGame::BJGame() {
    dealer Dealer;
    gDeck.populate();
}

void BJGame::startGame(std::vector<abstractGamePlayer*> players)
{
    PlayersInGame = players;
}

std::vector<abstractGamePlayer *> BJGame::getPlayers()
{
    return PlayersInGame;
}

void BJGame::starting_deal()
{
    //TODO this is where you would get the prebet Deck
    //Get all players bets and deal hand
    std::for_each(PlayersInGame.begin(), PlayersInGame.end(), [this](abstractGamePlayer *player){
        player->newHand(player->getBet());
        gDeck.deal(*player, 2);
    });

    gDeck.deal(Dealer,2);
    Dealer.getCard(0).flip();
    // TODO check if we need to show output
    Dealer.print();

}