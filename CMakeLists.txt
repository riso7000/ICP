cmake_minimum_required(VERSION 3.5)
project(petri-net-ed)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

if(WIN32)
    set(APP_ICON_RESOURCE res/icon.rc)
endif()

qt5_add_resources(RESOURCES res/resources.qrc)

add_executable(petri-net-ed
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
    res/resources.qrc
    ${APP_ICON_RESOURCE}
)

target_link_libraries(petri-net-ed
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
)

target_include_directories(petri-net-ed PRIVATE
    third_party/Cflat
)
