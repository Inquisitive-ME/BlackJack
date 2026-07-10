# GoogleTest: prefer a system install; otherwise fetch a pinned release.
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.15.2)
    # Prevent overriding the parent project's compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

include(GoogleTest)

# All simulator sources except the CLI entry point, shared by every test executable.
file(GLOB SIM_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp)
list(FILTER SIM_SOURCES EXCLUDE REGEX ".*main\\.cpp$")

find_package(SQLite3 REQUIRED)

function(add_bj_test name)
    add_executable(${name} ${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp ${SIM_SOURCES})
    target_include_directories(${name} PRIVATE ${SQLite3_INCLUDE_DIRS})
    target_link_libraries(${name} PRIVATE ${SQLite3_LIBRARIES} GTest::gtest_main GTest::gmock_main)
    gtest_discover_tests(${name})
endfunction()

add_bj_test(cardTest)
add_bj_test(handTest)
add_bj_test(BJHandTest)
add_bj_test(deckTest)
add_bj_test(dealerTest)
add_bj_test(playerTest)
add_bj_test(randomTest)
add_bj_test(BJGameTest)
add_bj_test(blackjack_database_test)

# --- Fast-core (bjsim) tests: no SQLite, no legacy blob ---
function(add_sim_test name)
    add_executable(${name} sim/${name}.cpp ${ARGN})
    target_link_libraries(${name} PRIVATE bjsim GTest::gtest_main GTest::gmock_main)
    gtest_discover_tests(${name})
endfunction()

# shoe_test links legacy card.cpp only to pin the new count tables against it.
add_sim_test(shoe_test ${CMAKE_CURRENT_SOURCE_DIR}/../src/card.cpp)
add_sim_test(hand_test)
add_sim_test(dealer_test)
add_sim_test(round_test)
add_sim_test(strategy_test)
add_sim_test(sim_engine_test)
add_sim_test(count_play_test)
