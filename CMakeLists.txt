cmake_minimum_required(VERSION 3.16)
project(ACE VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

# --- 1. GUARDIAN INTEGRATION (RE-ADDED) ---
# This runs the Python script to format and sync the tree before building
find_package(Python3 REQUIRED)
message(STATUS "Guardian: Checking for source changes...")
execute_process(
    COMMAND ${Python3_EXECUTABLE} "/home/user/ACE/guardian.py"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE GUARDIAN_RESULT
)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(SDL3 REQUIRED)

# --- 2. AUTOMATIC FOLDER DISCOVERY ---
macro(add_all_subdirectories_to_includes target_name search_path)
    file(GLOB_RECURSE children LIST_DIRECTORIES true "${search_path}/*")
    set(dir_list "")
    foreach(child ${children})
        if(IS_DIRECTORY ${child})
            list(APPEND dir_list ${child})
        endif()
    endforeach()
    target_include_directories(${target_name} PRIVATE ${dir_list})
endmacro()

# --- 3. GATHER SOURCES AFTER GUARDIAN RUNS ---
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h" "include/*.h")

add_executable(ACE ${SOURCES} ${HEADERS})

# Add the base directories
target_include_directories(ACE PRIVATE src include)

# Automatically add every single subfolder in include/ and src/
add_all_subdirectories_to_includes(ACE "${CMAKE_SOURCE_DIR}/include")
add_all_subdirectories_to_includes(ACE "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(ACE PRIVATE Qt6::Widgets SDL3::SDL3 pthread)

target_precompile_headers(ACE PRIVATE 
    <QtWidgets/QWidget> <QtWidgets/QMainWindow> <SDL3/SDL.h> 
    <iostream> <atomic> <vector>
)