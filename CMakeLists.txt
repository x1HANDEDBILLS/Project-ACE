cmake_minimum_required(VERSION 3.16)
project(ACE VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(SDL3 REQUIRED)

# We use a glob to find files, but we REMOVED CONFIGURE_DEPENDS to stop the loops
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h" "include/*.h")

add_executable(ACE ${SOURCES} ${HEADERS})

target_include_directories(ACE PRIVATE 
    src include include/Core include/theme include/widgets include/input 
    include/logic include/dash include/windows include/rc_type 
    include/profiles include/vehicles/plane include/vehicles/drone 
    include/vehicles/heli include/vehicles/car include/protocols
)

target_link_libraries(ACE PRIVATE Qt6::Widgets SDL3::SDL3 pthread)

target_precompile_headers(ACE PRIVATE 
    <QtWidgets/QWidget> <QtWidgets/QMainWindow> <SDL3/SDL.h> 
    <iostream> <atomic> <vector>
)
