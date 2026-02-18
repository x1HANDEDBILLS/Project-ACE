cmake_minimum_required(VERSION 3.10)
project(ProjectACE)

# Standard C++17 for performance and modern features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Enable Qt's automatic code generators
# These are required to handle the Q_OBJECT macro in logic.h and the resources.qrc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 2. Find Qt5 Packages
# These must be installed via: sudo apt install qtbase5-dev qtdeclarative5-dev qtquickcontrols2-5-dev
find_package(Qt5 COMPONENTS Core Gui Quick REQUIRED)

# 3. Automatically find all .cpp and .h files recursively
# This includes your main.cpp, everything in src/, and your resource file
file(GLOB_RECURSE SOURCES 
    "main.cpp"
    "src/*.cpp"
    "src/*.h"
    "resources.qrc"
)

# 4. Create the executable target
add_executable(ace_engine ${SOURCES})

# 5. Define Include Directories
# This allows you to use #include "logic.h" without typing the full path
target_include_directories(ace_engine PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/logic 
    ${CMAKE_SOURCE_DIR}/src/input 
    ${CMAKE_SOURCE_DIR}/src/transmit_type 
    ${CMAKE_SOURCE_DIR}/src/rc_type
)

# 6. Link Libraries
# Links the Qt Framework, Pthreads for multi-threading, and RT for real-time clock access
target_link_libraries(ace_engine 
    Qt5::Core 
    Qt5::Gui 
    Qt5::Quick 
    pthread 
    rt
)

# 7. Raspberry Pi 4/5 Optimization Flags
# Tells the compiler to optimize for the specific silicon on your board
target_compile_options(ace_engine PRIVATE -O3 -march=native -mtune=native)