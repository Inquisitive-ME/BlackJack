# Specify the cross compiler
SET(CMAKE_C_COMPILER ${CMAKE_CURRENT_LIST_DIR}/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-gcc-4.8.3)
SET(CMAKE_CXX_COMPILER ${CMAKE_CURRENT_LIST_DIR}/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-g++)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mcpu=cortex-a53  -mfpu=neon-fp-armv8")

# Specify the target environment
SET(CMAKE_FIND_ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/tools/rootfs")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")
SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} --sysroot=${CMAKE_FIND_ROOT_PATH}")

# Set where to allow cmake to search for stuff
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

include_directories("${CMAKE_CURRENT_LIST_DIR}/tools/rootfs/usr/include/"
        "${CMAKE_CURRENT_LIST_DIR}/tools/rootfs//usr/include/arm-linux-gnueabihf/")

#set(CMAKE_SYSROOT ${RASPI_SYSROOT})
#include_directories(${RASPI_SYSROOT}/usr/include/arm-linux-gnueabihf)
