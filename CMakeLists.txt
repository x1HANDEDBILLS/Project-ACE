cmake_minimum_required(VERSION 3.16)

project(Project-ACE VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt6
find_package(Qt6 6.5 REQUIRED COMPONENTS Gui Quick)

# SDL3 - required for gamepad support in input_read.cpp / input_manage.cpp
find_package(SDL3 REQUIRED)

# System libraries
find_package(Threads REQUIRED)
find_library(RT_LIBRARY rt)
find_library(UDEV_LIBRARY udev)
find_library(EVDEV_LIBRARY evdev)

qt_standard_project_setup(REQUIRES 6.5)

add_executable(ace_engine
    main.cpp
    src/logic/logic.cpp
    src/input/input_read.cpp
    src/input/input_manage.cpp   # ← added this missing file
    resources.qrc
)

target_include_directories(ace_engine PRIVATE
    ${CMAKE_SOURCE_DIR}/src/logic
    ${CMAKE_SOURCE_DIR}/src/input
)

target_link_libraries(ace_engine PRIVATE
    Qt6::Gui
    Qt6::Quick
    SDL3::SDL3               # ← added SDL3
    Threads::Threads
    ${RT_LIBRARY}
    ${UDEV_LIBRARY}
    ${EVDEV_LIBRARY}
)

qt_add_qml_module(ace_engine
    URI AceModule
    VERSION 1.0
    QML_FILES main.qml
)