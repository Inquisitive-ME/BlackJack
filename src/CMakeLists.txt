cmake_minimum_required(VERSION 3.10)

project(BlackJack)

set(SOURCE_FILES
        main.cpp
        card.cpp
		hand.cpp
		bj_hand.cpp
		player_impl.cpp
		deck.cpp
		dealer_impl.cpp
		bj_game.cpp
		get_random_int.cpp
        blackjack_database.cpp
		commandline_ai_impl.cpp
		command_line_player.cpp)

find_package(SQLite3)
include_directories(${SQLite3_INCLUDE_DIRS})

add_executable(BlackJack ${SOURCE_FILES})
target_link_libraries(BlackJack ${SQLite3_LIBRARIES})
