cmake_minimum_required(VERSION 3.12)
project(test2)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV 4.9.0 REQUIRED)

message("OpenCV version: " ${OpenCV_VERSION})

include_directories(${OpenCV_INCLUDE_DIRS})

link_directories(${OpenCV_LIB_DIR})

set(SRC main.cpp)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

message("${OpenCV_INCLUDE_DIRS}")