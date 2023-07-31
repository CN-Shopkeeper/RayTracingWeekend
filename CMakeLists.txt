cmake_minimum_required(VERSION 3.1.0)

project(
    SimpleRayTracing
    LANGUAGES CXX
)

include(cmake/copyresources.cmake)

# Set to c++11
set(CMAKE_CXX_STANDARD 11)

include_directories(src/common)

add_subdirectory(src/inOneWeekend)
add_subdirectory(src/theNextWeek)
add_subdirectory(src/theRestOfYourLife)