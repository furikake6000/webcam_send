cmake_minimum_required(VERSION 2.8.3)

project(webcam_send)

add_definitions(-std=c++11)

find_package(OpenCV REQUIRED)

find_package(GFlags)

find_package(Glog)

find_package(OpenPose REQUIRED)

include_directories(
  ${OpenCV_INCLUDE_DIRS}
  ${OpenPose_INCLUDE_DIRS}
)

add_executable(webcam_recv webcam_recv.cpp)

target_link_libraries(webcam_recv ${OpenCV_LIBRARIES} ${OpenPose_LIBS})
