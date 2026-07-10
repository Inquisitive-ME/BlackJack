cmake_minimum_required(VERSION 3.20)

project(BlackJack)

# Header-only fast simulation core. Consumers include "sim/shoe.h" etc.
add_library(bjsim INTERFACE)
target_include_directories(bjsim INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(bjsim INTERFACE cxx_std_23)

# Console game: a thin human front-end over the core engine.
add_executable(BlackJack main.cpp)
target_link_libraries(BlackJack PRIVATE bjsim)
