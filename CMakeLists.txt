cmake_minimum_required(VERSION 3.10)
project(RLZ)
set(CMAKE_BUILD_TYPE Release)

set(RLZ_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/include/ ${PROJECT_BINARY_DIR}/include/)
include_directories(${RLZ_INCLUDE_DIRS})
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_library(RLZ SHARED ${SOURCES})
target_include_directories(RLZ PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
