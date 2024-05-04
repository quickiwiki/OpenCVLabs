cmake_minimum_required(VERSION 3.12)
project(App6)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV 4.9.0 REQUIRED)
find_package(OpenGL REQUIRED)
#find_package(GLUT REQUIRED)


message("OpenCV version: " ${OpenCV_VERSION})
message(${OpenCV_INCLUDE_DIRS})

set(SRC main.cpp)

add_executable(${PROJECT_NAME} ${SRC})
link_directories(${OpenCV_LIB_DIR})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})
