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
function(test name addition_run_arguements)
    string(REPLACE "/" "_" out ${name})
    add_executable(${out} ${CMAKE_CURRENT_LIST_DIR}/${name}.cpp ${SRC_FILES})
    #add_test(${out} ./${out})
    # gtest_discover_tests will only run tests that change
    gtest_discover_tests(${out} TEST_PREFIX ${scenario}:)
    target_link_libraries(${out} ${SQLITE} gtest_main gmock_main)
    add_custom_command(TARGET ${out} COMMAND ./${out} ${addition_run_arguements})
endfunction()

include(GoogleTest)

test(cardTest "")
test(handTest "")
test(BJHandTest "")
test(deckTest "")
test(dealerTest "")
test(playerTest "")
test(randomTest "")
test(BJGameTest "")
test(blackjack_database_test "")