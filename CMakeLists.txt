cmake_minimum_required(VERSION 3.10)
project(xACE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_library(UDEV_LIB udev REQUIRED)
# 1. New: Find libevdev (usually located in /usr/include/libevdev-1.0)
find_path(LIBEVDEV_INCLUDE_DIR libevdev/libevdev.h PATH_SUFFIXES libevdev-1.0)
find_library(LIBEVDEV_LIB evdev REQUIRED)

include_directories(${CMAKE_SOURCE_DIR} ${LIBEVDEV_INCLUDE_DIR})

add_executable(ace_engine 
    main.cpp 
    src/logic/logic.cpp
    src/input/input_read.cpp
)

target_link_libraries(ace_engine PRIVATE 
    Threads::Threads
    rt
    ${UDEV_LIB}
    ${LIBEVDEV_LIB} # 2. New: Link the library
)