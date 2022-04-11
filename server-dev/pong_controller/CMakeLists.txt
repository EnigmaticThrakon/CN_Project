cmake_minimum_required(VERSION 3.20)
project(pong_controller)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(hiredis REQUIRED)

# file(COPY ${CMAKE_SOURCE_DIR}/../shared_files DESTINATION ${CMAKE_SOURCE_DIR})

add_executable(pong_controller pong_controller.cpp)
target_link_libraries(pong_controller ${HIREDIS_LIBRARY})
