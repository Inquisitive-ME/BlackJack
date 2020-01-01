SET(CMAKE_SYSTEM_NAME Linux)
cmake_minimum_required(VERSION 3.10)
project(BlackJack)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall" )
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -02")

# get compiler for Raspberry Pi
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
if("${PLATFORM}" STREQUAL "LINUX_X86_GCC")
    add_subdirectory(test)
endif()
