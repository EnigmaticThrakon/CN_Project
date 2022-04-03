cmake_minimum_required(VERSION 3.20)
project(testing)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(hiredis REQUIRED)

file(COPY ${CMAKE_SOURCE_DIR}/../../redis_shared_files DESTINATION ${CMAKE_SOURCE_DIR})

add_executable(testing pong_controller.cpp)
target_link_libraries(testing ${HIREDIS_LIBRARY})
