if(EXISTS ${CMAKE_CURRENT_BINARY_DIR}/googletest-download/CMakeLists.txt)
else()
    # Download and unpack googletest at configure time
    configure_file(includeGTest.cmake googletest-download/CMakeLists.txt)
    execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "CMake step for googletest failed: ${result}")
    endif()
    execute_process(COMMAND ${CMAKE_COMMAND} --build .
            RESULT_VARIABLE result
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
    if(result)
        message(FATAL_ERROR "Build step for googletest failed: ${result}")
    endif()
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

file(GLOB SRC_FILES ../src/*.cpp)
list(FILTER SRC_FILES EXCLUDE REGEX ".*main.cpp$")

find_library(SQLITE sqlite3)
function(test name scenario)
    # scenario is additional google test run parameters
    string(REPLACE "/" "_" out ${name})
    add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp ${SRC_FILES})
    #add_test(${out} ./${out})
    # gtest_discover_tests will only run tests that change
    gtest_discover_tests(${out} TEST_PREFIX ${scenario}:)
    target_link_libraries(${out} ${SQLITE} gtest_main gmock_main)
    add_custom_command(TARGET ${out} COMMAND ./${out} ${scenario})
endfunction()

include(GoogleTest)
# TODO There has to be a better way to add the files
# Now simply link against gtest or gtest_main as needed. Eg
#add_executable(BJGameTest BJGameTest.cpp ../src/BJGame.cpp ../src/player.cpp ../src/hand.cpp ../src/BJHand.cpp ../src/deck.cpp ../src/dealer.cpp ../src/card.cpp ../src/getRandomInt.cpp)
#target_link_libraries(BJGameTest gtest gmock)
#add_test(NAME example_test COMMAND BJGameTest)
#add_custom_command(TARGET BJGameTest COMMAND ./${out})
#test(BJGameTest "--gtest_also_run_disabled_tests")
test(BJGameTest "")
test(cardTest "")
test(handTest "")
test(BJHandTest "")
test(dealerTest "")
test(playerTest "")
test(randomTest "")
test(blackjack_database_test "")
#link_directories(${GUNIT_LINK_DIRS})
#test(cardTest SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/cardTest.feature)
#target_link_libraries(cardTest ${CMAKE_CURRENT_BINARY_DIR}/../src/libBlackJack_Testing.a)
#
#test(handTest SCENARIO="")
#target_link_libraries(handTest ${CMAKE_CURRENT_BINARY_DIR}/../src/libBlackJack_Testing.a)
#
#test(BJHandTest SCENARIO="")
#target_link_libraries(BJHandTest ${CMAKE_CURRENT_BINARY_DIR}/../src/libBlackJack_Testing.a)
#
#test(randomTest SCENARIO="")
#target_link_libraries(randomTest ${CMAKE_CURRENT_BINARY_DIR}/../src/libBlackJack_Testing.a)
#
#test(playerTest SCENARIO="")
#target_link_libraries(playerTest ${CMAKE_CURRENT_BINARY_DIR}/../src/libBlackJack_Testing.a)
#
#test(deckTest SCENARIO="")
#target_link_libraries(deckTest ${CMAKE_CURRENT_BINARY_DIR}/../src/libBlackJack_Testing.a)
#
#test(dealerTest SCENARIO="")
#target_link_libraries(dealerTest ${CMAKE_CURRENT_BINARY_DIR}/../src/libBlackJack_Testing.a)