cmake_minimum_required(VERSION 3.16)
project(Project-ACE VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL3 for gamepad support
find_package(SDL3 REQUIRED)

# System libraries for Real-Time and Threads
find_package(Threads REQUIRED)
find_library(RT_LIBRARY rt)

add_executable(ace_engine
    main.cpp
    src/logic/logic.cpp
    src/logic/logic2.cpp
    src/input/input_read.cpp
    src/input/input_manage.cpp
    src/shared/socket.cpp
)

target_include_directories(ace_engine PRIVATE
    ${CMAKE_SOURCE_DIR}/src/logic
    ${CMAKE_SOURCE_DIR}/src/input
    ${CMAKE_SOURCE_DIR}/src/shared
)

target_link_libraries(ace_engine PRIVATE
    SDL3::SDL3
    Threads::Threads
    ${RT_LIBRARY}
)

# Optimization for performance
target_compile_options(ace_engine PRIVATE -O3 -Wall)