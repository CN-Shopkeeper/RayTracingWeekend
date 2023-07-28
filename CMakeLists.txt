cmake_minimum_required(VERSION 3.1.0)

project(
    SimpleRayTracing
    LANGUAGES CXX
)

# Set to c++11
set(CMAKE_CXX_STANDARD 11)

include_directories(src/common)

add_subdirectory(src/inOneWeekend)