cmake_minimum_required(VERSION 3.20)
project(port_listener)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(hiredis REQUIRED)
find_package(Threads REQUIRED)

file(COPY ${CMAKE_SOURCE_DIR}/../redis_shared_files DESTINATION ${CMAKE_SOURCE_DIR})

add_executable(port_listener socket-listener.cpp)
target_link_libraries(port_listener PRIVATE ${HIREDIS_LIBRARY})
target_link_libraries(port_listener PRIVATE Threads::Threads)