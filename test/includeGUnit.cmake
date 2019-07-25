cmake_minimum_required(VERSION 3.10)

project(GUnit-download NONE)

include(ExternalProject)
ExternalProject_Add(GUnit
        GIT_REPOSITORY    https://github.com/cpp-testing/GUnit.git
        GIT_TAG           master
        SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/GUnit-src"
        BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/GUnit-build"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND     ""
        INSTALL_COMMAND   ""
        TEST_COMMAND      ""
        )