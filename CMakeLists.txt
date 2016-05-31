cmake_minimum_required(VERSION 3.3)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(libmc)
set(CMAKE_BUILD_TYPE Debug)

option(BUILD_SAMPLES "Build the sample programs" ON)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(SYSTEM
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    ${GLM_INCLUDE_DIR}
    )

add_subdirectory("./src")
