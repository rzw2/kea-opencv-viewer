cmake_minimum_required(VERSION 3.4 FATAL_ERROR)

project(kea_opencv_viewer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

find_package(tof REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)
find_package(OpenMP REQUIRED)


set(kea_opencv_viewer_srcs src/main.cpp src/colormap.cpp)

include_directories(${OpenCV_INCLUDE_DIRS}) 

add_executable(kea_opencv_viewer ${kea_opencv_viewer_srcs})

target_link_libraries(kea_opencv_viewer PRIVATE OpenMP::OpenMP_CXX tof::tof
                                                ${OpenCV_LIBS})

install(TARGETS kea_opencv_viewer DESTINATION ${CMAKE_INSTALL_BINDIR})

