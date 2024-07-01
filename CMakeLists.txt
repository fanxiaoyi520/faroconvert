cmake_minimum_required(VERSION 3.5)

project(FaroManager LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

list(APPEND QML_DIRS "${CMAKE_SOURCE_DIR}") #qmldir路径
file(GLOB SRC_FILES "${SRC_DIR}/*.cpp")
file(GLOB SRC_ICON_FILES "${SRC_DIR}/*.rc")
file(GLOB INC_FILES "${SRC_DIR}/*.h")

# 将源文件和头文件添加到项目中
foreach(SRC_FILE ${SRC_FILES})
    set_property(SOURCE ${SRC_FILE} PROPERTY LANGUAGE CXX)
    list(APPEND PROJECT_SOURCES ${SRC_FILE})
endforeach()

foreach(INC_FILE ${INC_FILES})
    list(APPEND PROJECT_HEADERS ${INC_FILE})
endforeach()

# 查找 Qt 包
find_package(Qt5 REQUIRED COMPONENTS Core Concurrent)
find_package(Qt5 COMPONENTS Core Quick Sql REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Network)

# 包含头文件路径
include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Concurrent_INCLUDE_DIRS})

add_executable(FaroManager
  main.cpp
  ${PROJECT_SOURCES} ${PROJECT_HEADERS}
)
target_link_libraries(FaroManager Qt5::Core Qt5::Concurrent Qt5::Quick)
