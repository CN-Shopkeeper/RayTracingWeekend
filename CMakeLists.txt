cmake_minimum_required(VERSION 3.1.0)

project(
    SimpleRayTracing
    LANGUAGES CXX
)

# Set to c++11
set(CMAKE_CXX_STANDARD 11)

aux_source_directory(src/inOneWeekend SourceInOneWeekend)
add_executable(inOneWeekend ${SourceInOneWeekend})

aux_source_directory(src/theNextWeek SourceTheNextWeek)
add_executable(theNextWeek ${SourceTheNextWeek})

include_directories(src/common)