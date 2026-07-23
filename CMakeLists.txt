cmake_minimum_required(VERSION 3.18 FATAL_ERROR)

project(py-cpp-data-binder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v3.0.3
)
FetchContent_MakeAvailable(pybind11)

add_executable(main main.cpp)

target_link_libraries(main PRIVATE pybind11::embed)

add_executable(benchmark benchmark/benchmark.cpp)

target_link_libraries(benchmark PRIVATE pybind11::embed)