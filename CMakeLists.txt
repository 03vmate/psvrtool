cmake_minimum_required(VERSION 3.10)
project(psvrtool)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(libusb-1.0 REQUIRED)

add_executable(psvrtool main.cpp)

target_include_directories(psvrtool PRIVATE ${LIBUSB_1_INCLUDE_DIRS})
target_link_libraries(psvrtool PRIVATE ${LIBUSB_1_LIBRARIES})
