cmake_minimum_required(VERSION 3.24)
project(scene_name_parser)

set(CMAKE_CXX_STANDARD 23)


option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # doctest
    include_directories(extern/doctest)
    add_executable(scene_name_parser_tests name_parsing_tests.cpp)
endif()

add_executable(scene_name_parser main.cpp scene-name-parser.hpp)
