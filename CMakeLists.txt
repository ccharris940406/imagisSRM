cmake_minimum_required(VERSION 3.15)
project(imagisSRM)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED Xml PrintSupport Core Concurrent)
find_package(DCMTK REQUIRED)
include_directories(${DCMTK_INCLUDE_DIRS})


add_executable(imagisSRM
        main.cpp
        resources.qrc
        GUI/imgSRWidget.cpp
        GUI/imgSRWidget.h
        GUI/imgSRWidget.ui
        GUI/imgSideWidgetElement.cpp
        GUI/imgSideWidgetElement.h
        GUI/imgSideWidgetElement.ui
        CORE/imgSRDataSetHandler.cpp CORE/imgSRDataSetHandler.h CORE/utils.h GUI/imgPendingFileHandler.cpp GUI/imgPendingFileHandler.h GUI/imgXMLMaker.cpp GUI/imgXMLMaker.h GUI/imgSendThread.cpp GUI/imgSendThread.h CORE/imgSRCstore.cpp CORE/imgSRCstore.h
        CORE/imgSRQuery.cpp CORE/imgSRQuery.h GUI/imgQueryDialog.cpp GUI/imgQueryDialog.h
        GUI/imgImagePickerDialog.cpp GUI/imgImagePickerDialog.h
        GUI/imgReportPickerDialog.cpp GUI/imgReportPickerDialog.h)

target_link_libraries(imagisSRM PRIVATE Qt5::Widgets Qt5::Xml Qt5::PrintSupport Qt5::Core Qt5::Concurrent DCMTK::DCMTK)