# TODO: Move the routines from mc_algorithms_simple to this library
add_library(mc_algorithms_common STATIC
    cube.c
    surfaceNet.c
    )
target_include_directories(mc_algorithms_common SYSTEM
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
    )
