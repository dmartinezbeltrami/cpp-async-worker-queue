cmake_minimum_required(VERSION 3.10)
project(cpp_async_worker_queue CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(worker_queue INTERFACE)
target_include_directories(worker_queue INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(simple_example examples/simple_example.cpp)
target_link_libraries(simple_example PRIVATE worker_queue)