cmake_minimum_required(VERSION 2.8)

set(PROJECT yac)

project(${PROJECT})

set (MAJOR_VERSION  0)
set (MINOR_VERSION  1)
option(${PROJECT}_TESTS "Enable tests for ${PROJECT}" On)

include_directories(src)

add_executable(${PROJECT} yac_cli.cpp)

if(${PROJECT}_TESTS)
    add_subdirectory(test)
    add_dependencies(${PROJECT} unit_test)
endif()
