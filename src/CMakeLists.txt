cmake_minimum_required(VERSION 3.10)

project(BlackJack)

set(SOURCE_FILES
        main.cpp
        card.cpp
		hand.cpp
		BJHand.cpp
		player_impl.cpp
		deck.cpp
		dealer.cpp
		bj_game.cpp
		getRandomInt.cpp
        blackjack_database.cpp)

find_library(SQLITE sqlite3)

add_executable(BlackJack ${SOURCE_FILES})
target_link_libraries(BlackJack ${SQLITE})
