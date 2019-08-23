cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)

project(ComputerVision)

#set the path for the executable vision program on the Top Folder (Source Directory)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/exe)

find_package(CUDA)
find_package(OpenCV)
find_package(Qt4)

include(${QT_USE_FILE})
include_directories( ${OpenCV_INCLUDE_DIRS})
add_definitions(${QT_DEFINITIONS})

cuda_add_executable(ComputerVision main.cpp FridoPipeline.cpp)

target_link_libraries(ComputerVision ${OpenCV_LIBS})
