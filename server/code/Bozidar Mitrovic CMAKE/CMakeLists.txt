cmake_minimum_required(VERSION 3.16.3)

set (CMAKE_AUTOMOC ON)
set (CMAKE_INCLUDE_CURRENT_DIR ON)

project(server VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++0x")
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})

set(SOURCES
    code/src/main.cpp
    code/src/tcpconnection.cpp
    code/headers/tcpconnection.h
    code/src/tcprunnable.cpp
    code/headers/tcprunnable.h
    code/src/tcpserver.cpp
    code/headers/tcpserver.h
)

add_executable(test ${SOURCES} )


target_link_libraries(test PUBLIC
    Qt5::Core
    Qt5::Network
)
