# Sanitized embedded Linux CMake toolchain example.
#
# Copy this file outside the repository or pass values through environment
# variables. Do not commit private SDK paths, credentials, board bundles, or
# generated sysroots.

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR "$ENV{BOARD_ARCH}" CACHE STRING "Target CPU architecture")

set(CMAKE_SYSROOT "$ENV{BOARD_SYSROOT}" CACHE PATH "Target sysroot")
set(CMAKE_C_COMPILER "$ENV{BOARD_TOOLCHAIN_PREFIX}gcc" CACHE FILEPATH "Target C compiler")
set(CMAKE_CXX_COMPILER "$ENV{BOARD_TOOLCHAIN_PREFIX}g++" CACHE FILEPATH "Target C++ compiler")

set(CMAKE_FIND_ROOT_PATH
    "${CMAKE_SYSROOT}"
    "$ENV{BOARD_QT_CMAKE_PREFIX}"
)

set(CMAKE_PREFIX_PATH "$ENV{BOARD_QT_CMAKE_PREFIX}" CACHE PATH "Board Qt CMake package prefix")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
