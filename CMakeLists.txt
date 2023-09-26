cmake_minimum_required(VERSION 3.14)

project(cpp-deepdive VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES main.cpp
        src/interval_map.h
        src/types.h
        src/numeric_limits_specialization.h)

add_library(cpp-deepdive ${SOURCES})
target_include_directories(cpp-deepdive PUBLIC src)

add_executable(main main.cpp)
target_include_directories(main PUBLIC src)

enable_testing()

add_subdirectory(tests)
