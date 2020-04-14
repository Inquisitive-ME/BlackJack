//
// Created by richard on 7/24/19.
//

#include <iostream>
#include "card.h"
#include "bj_hand.h"
#include "command_line_player.h"
#include "get_random_int.h"
#include <iostream>
#include <chrono>
#include "bj_game.h"
#include "commandline_ai_impl.h"
#include "command_line_dealer.h"

using namespace std;

int ask_number(std::string question, int low,int high)
{
    int response = low - 1;
    cout << question << endl;
    cin >> response;

    if(response >= low && response <=high)
        return response;
    else
    {
        cout << "Please enter a number between " << low << " and " << high << endl;
        cin.clear();
        std::string ignoreLine; //read the invalid input into it
        std::getline(cin, ignoreLine); //read the line till next space
        return ask_number(question,low,high);
    }
}

std::vector<PlayerImpl*> get_players_list()
{
    int numPlayersInput;
    std::cout << "How many players?" << std::endl;
    std::cin >> numPlayersInput;

    std::vector<PlayerImpl*> playersList;

    for(int i = 0; i < numPlayersInput; i++){
        string pName;
        std::cout << "What is your name?" << std::endl;
        std::cin >> pName;
        std::cout << pName << " ";
        int pPurse = ask_number("how much money will you start with?",0,100);
        PlayerImpl* player = new CommandLinePlayer(pName, pPurse);
        playersList.push_back(player);
    }

    return playersList;
}

int main() {
    card testCard(11);
    BJHand testHand(0);
    PlayerImpl testPlayer;

    cout << "Created Card " << testCard.print() << endl;
    testHand.copy_to_hand(testCard);
    cout << "Test Hand: " << testHand.print() << endl;
    testPlayer.getHands().push_back(testHand);
    cout << "test Players Hand = " << testPlayer.getHands()[0].print() << endl;

    int numNumberToGenerate = 100000;

    auto start = std::chrono::high_resolution_clock::now();

    int count[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 0; i < numNumberToGenerate; i++) {
        int test = getRandomInt(0, 9);
        count[test] = count[test] + 1;
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;

    cout << "Took " << elapsed.count() << " seconds to generate " << numNumberToGenerate << " random numbers" << endl;

    // Initialize player list for game
    std::vector<PlayerImpl*> gamePlayers = get_players_list();
    for(auto i : gamePlayers){
        std:: cout << i->getName() << std::endl;
    }
    CommandLineAI AI;
    CommandLineDealer Dealer;

    BJGame main_game{Dealer, {gamePlayers.begin(), gamePlayers.end()}, AI};

    main_game.printDeck();
    main_game.play();


    return 0;
}
