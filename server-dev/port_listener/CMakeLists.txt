cmake_minimum_required(VERSION 3.20)
project(port_listener)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(hiredis REQUIRED)
find_package(Threads REQUIRED)

add_executable(port_listener socket-listener-temp.cpp)
target_link_libraries(port_listener PRIVATE ${HIREDIS_LIBRARY})
target_link_libraries(port_listener PRIVATE Threads::Threads)