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

add_executable(bind test_bind.cpp)

target_link_libraries(bind PRIVATE pybind11::embed)