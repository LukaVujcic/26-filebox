cmake_minimum_required(VERSION 3.10.2)
project(TCPServer VERSION 1.0.0 LANGUAGES CXX)

include_directories(headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdPartyLibraries/zipper)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_COMPILER "g++-10")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-g")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(SOURCES
    src/main.cpp
    src/tcpconnection.cpp
    src/tcprunnable.cpp
    src/tcpserver.cpp
)
set(HEADERS
    headers/tcpconnection.h
    headers/tcprunnable.h
    headers/tcpserver.h
)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/thirdPartyLibraries/zipper/build/libZipper.so)
qt5_use_modules(${PROJECT_NAME} Core  Widgets MultimediaWidgets)