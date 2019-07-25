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
        ${CMAKE_SOURCE_DIR}/src)

include_directories(${GUNIT_INCLUDE_DIRS})

set(GUNIT_LINK_DIRS
        ${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/libs/gherkin-cpp
        )
message(${GUNIT_LINK_DIRS})
message(${CMAKE_SOURCE_DIR}/src)

#find_file(GHERKIN_LIBRARY_EXISTS libgherkin-cpp.a PATHS ${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/libs/gherkin-cpp/)
#message("GHERKIN_LIBRARY_EXISTS ${GHERKIN_LIBRARY_EXIlSTS}")
## make sure libgherkin-cpp.a exists
#if(EXISTS GHERKIN_LIBRARY_EXISTS)
#else()
#    message("${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/libs/gherkin-cpp")
#    execute_process(COMMAND make lib-static
#            RESULT_VARIABLE result
#            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/libs/gherkin-cpp )
#    if(result)
#        message(FATAL_ERROR "Failed to compile libgherkin-cpp.a: ${result}")
#    endif()
#endif()
#
#
#add_custom_command(
#        OUTPUT libgherkin-cpp-static
#        COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/libs/gherkin-cpp && make lib-static
#)
#
#find_library(GHERKIN_LIB gherkin-cpp NAMES libgherkin-cpp.a PATHS ${CMAKE_CURRENT_BINARY_DIR}/GUnit-src/libs/gherkin-cpp)
#message(${GHERKIN_LIB})

# Now simply link against gtest or gtest_main as needed. Eg
#add_executable(cardTest cardTest.cpp ../src/card.cpp)
#target_link_libraries(cardTest gtest_main)
#add_test(NAME card_test COMMAND cardTest)

link_directories(${GUNIT_LINK_DIRS})
test(cardTest SCENARIO=${CMAKE_CURRENT_SOURCE_DIR}/cardTest.feature)
