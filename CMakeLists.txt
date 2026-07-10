cmake_minimum_required(VERSION 3.20)
project(BlackJack)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

# get compiler for Raspberry Pi
# Didn't use cmakes git add submodule because the directory needs to be there for cmake before building
if("${PLATFORM}" STREQUAL "rpi3b+")
    if(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tools)
        execute_process(COMMAND git clone https://github.com/raspberrypi/tools.git ${CMAKE_CURRENT_SOURCE_DIR}/tools)
    endif()
    if(NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/)
        message(FATAL_ERROR "tools is expected to be git repo https://github.com/raspberrypi/tools.git")
    endif()
endif()

enable_testing()
add_subdirectory(src)
add_subdirectory(test)
