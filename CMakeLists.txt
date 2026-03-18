cmake_minimum_required(VERSION 3.16)
project(ACE VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(SDL3 REQUIRED)

# --- NEW: AUTOMATIC FOLDER DISCOVERY ---
# This function finds every subfolder so you never have to edit this file again
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

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h" "include/*.h")

add_executable(ACE ${SOURCES} ${HEADERS})

# Add the base directories first
target_include_directories(ACE PRIVATE src include)

# Automatically add every single subfolder in include/ and src/
add_all_subdirectories_to_includes(ACE "${CMAKE_SOURCE_DIR}/include")
add_all_subdirectories_to_includes(ACE "${CMAKE_SOURCE_DIR}/src")

target_link_libraries(ACE PRIVATE Qt6::Widgets SDL3::SDL3 pthread)

target_precompile_headers(ACE PRIVATE 
    <QtWidgets/QWidget> <QtWidgets/QMainWindow> <SDL3/SDL.h> 
    <iostream> <atomic> <vector>
)