cmake_minimum_required(VERSION 3.15)
project(imagisSRM)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ITK_DIR		   	/home/ccharris/Projects/CBM/libs/ITK/5.0.1/install/lib/cmake/ITK-5.0)

find_package(ITK)
include(${ITK_USE_FILE})
include_directories(${ITK_INCLUDE_PATH})

find_package(Qt5 COMPONENTS Widgets REQUIRED Xml PrintSupport Core Concurrent)


add_executable(imagisSRM
        main.cpp
        resources.qrc
        GUI/imgSRWidget.cpp
        GUI/imgSRWidget.h
        GUI/imgSRWidget.ui
        GUI/imgSideWidgetElement.cpp
        GUI/imgSideWidgetElement.h
        GUI/imgSideWidgetElement.ui
        CORE/imgSRDataSetHandler.cpp CORE/imgSRDataSetHandler.h CORE/utils.h GUI/imgPendingFileHandler.cpp GUI/imgPendingFileHandler.h GUI/imgXMLMaker.cpp GUI/imgXMLMaker.h GUI/imgSendThread.cpp GUI/imgSendThread.h CORE/imgSRCstore.cpp CORE/imgSRCstore.h)

target_link_libraries(imagisSRM PRIVATE Qt5::Widgets Qt5::Xml Qt5::PrintSupport Qt5::Core Qt5::Concurrent ${ITK_LIBRARIES})