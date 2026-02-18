cmake_minimum_required(VERSION 3.10)
project(xACE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find SDL3
find_package(SDL3 REQUIRED)

# 2. Include paths
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/logic
    ${CMAKE_SOURCE_DIR}/src/input
)

# 3. Executable and Sources
add_executable(ace_engine 
    main.cpp 
    src/logic/logic.cpp 
    src/input/input_read.cpp
    src/input/input_manage.cpp
)

# 4. Link Libraries
target_link_libraries(ace_engine PRIVATE SDL3::SDL3 pthread)