cmake_minimum_required(VERSION 3.1)
project(qt_petri)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(qt_petri
    src/main.cpp
)

target_link_libraries(qt_petri
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)
