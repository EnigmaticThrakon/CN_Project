cmake_minimum_required(VERSION 3.20)
project(socket_listener)
set(THREADS_PREFER_PTHREAD_FLAG ON)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(hiredis REQUIRED)
find_package(Threads REQUIRED)

add_executable(socket_listener socket_listener.cpp)
target_link_libraries(socket_listener PRIVATE ${HIREDIS_LIBRARY})
target_link_libraries(socket_listener PRIVATE Threads::Threads)