cmake_minimum_required(VERSION 3.10)

project(BlackJack)

set(SOURCE_FILES
        main.cpp
        card.cpp
		hand.cpp
		player.cpp
		getRandomInt.cpp)

add_library(BlackJack_Testing STATIC ${SOURCE_FILES})

add_executable(BlackJack ${SOURCE_FILES})
