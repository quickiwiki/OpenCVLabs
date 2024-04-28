cmake_minimum_required(VERSION 3.14)

project(test)
find_package(OpenCV 4.9.0 REQUIRED)
message("OpenCV Version: " ${OpenCV_VERSION})
include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${OpenCV_LIB_DIR})

set(SRC main.cpp)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
