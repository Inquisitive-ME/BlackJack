# Download and unpack GUnit at configure time
configure_file(includeGUnit.cmake GUnit-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/GUnit-download )
if(result)
    message(FATAL_ERROR "CMake step for GUnit failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/GUnit-download )
if(result)
    message(FATAL_ERROR "Build step for GUnit failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/GUnit-src
        ${CMAKE_CURRENT_BINARY_DIR}/GUnit-build
        EXCLUDE_FROM_ALL)

#export the include directory location as GUNIT_INCLUDE_DIR
set(GUNIT_INCLUDE_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/include
        ${gtest_SOURCE_DIR}/include
        ${gmock_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/libs/gherkin-cpp/include
        ${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/libs/json/src
        )

include_directories(${GUNIT_INCLUDE_DIRS})

set(GUNIT_LINK_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/libs/gherkin-cpp
        )

link_directories(${GUNIT_LINK_DIRS})
test(cardTest SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/cardTest.feature)
target_link_libraries(cardTest ${CMAKE_CURRENT_BINARY_DIR}/../src/libBlackJack_Testing.a)

test(handTest SCENARIO="")
target_link_libraries(handTest ${CMAKE_CURRENT_BINARY_DIR}/../src/libBlackJack_Testing.a)

