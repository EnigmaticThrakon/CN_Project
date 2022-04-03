cmake_minimum_required(VERSION 3.20)
project(port_listener)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(hiredis REQUIRED)

file(COPY ${CMAKE_SOURCE_DIR}/../../redis_shared_files DESTINATION ${CMAKE_SOURCE_DIR})

add_executable(port_listener socket-listener.cpp)
target_link_libraries(port_listener ${HIREDIS_LIBRARY})