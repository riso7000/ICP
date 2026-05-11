cmake_minimum_required(VERSION 3.5)
project(qt_petri)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(qt_petri
    src/main.cpp
    src/petriscene.cpp
    src/petriscene.h
    src/place.cpp
    src/place.h
    src/transition.cpp
    src/transition.h
    src/arc.cpp
    src/arc.h
    src/net_def_io.cpp
    src/net_def_io.hpp
    third_party/Cflat/Cflat.cpp
    src/properties_dialog.cpp
    src/properties_dialog.h
)

target_link_libraries(qt_petri
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

target_include_directories(qt_petri PRIVATE
    third_party/Cflat

)
