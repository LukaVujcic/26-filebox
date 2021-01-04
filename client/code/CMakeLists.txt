cmake_minimum_required(VERSION 3.10.2)
project(filebox VERSION 1.0.0 LANGUAGES CXX)

include_directories(headers)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdPartyLibraries/zipper)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
add_compile_options(-g)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../resources/images/images.qrc)

qt5_add_resources(RES_SOURCES ${RESOURCES})


set(SOURCES
    src/filebox.cpp
    src/filesystemtreeview.cpp
    src/login.cpp
    src/main.cpp
    src/register.cpp
    src/serverfilesbrowser.cpp
    src/tcpclient.cpp
)
set(HEADERS
    headers/filebox.h
    headers/filesystemtreeview.h
    headers/login.h
    headers/register.h
    headers/serverfilesbrowser.h
    headers/tcpclient.h
)
set(UI_FILES
    forms/filebox.ui
    forms/login.ui
    forms/register.ui
)

qt5_wrap_ui(UI_HEADERS ${UI_FILES})

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${UI_HEADERS} ${RES_SOURCES})
target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/thirdPartyLibraries/zipper/build/libZipper.so)
qt5_use_modules(${PROJECT_NAME} Core Gui Widgets MultimediaWidgets)