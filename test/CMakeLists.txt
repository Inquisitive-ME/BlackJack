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

# Every test is a self-contained sim/ test linking the header-only bjsim core.
function(add_sim_test name)
    add_executable(${name} sim/${name}.cpp)
    target_link_libraries(${name} PRIVATE bjsim GTest::gtest_main GTest::gmock_main)
    gtest_discover_tests(${name})
endfunction()

add_sim_test(shoe_test)
add_sim_test(hand_test)
add_sim_test(dealer_test)
add_sim_test(round_test)
add_sim_test(strategy_test)
add_sim_test(sim_engine_test)
add_sim_test(count_play_test)
