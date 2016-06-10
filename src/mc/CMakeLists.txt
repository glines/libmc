add_library(mc
    isosurfaceBuilder.c
    )
target_include_directories(mc SYSTEM
    PUBLIC "${CMAKE_SOURCE_DIR}/include"
    )
target_link_libraries(mc
    mc_algorithms_common
    mc_algorithms_cuberille
    mc_algorithms_cubes33
    mc_algorithms_delaunay
    mc_algorithms_elasticSurfaceNet
    mc_algorithms_patch
    mc_algorithms_simple
    mc_algorithms_snapmc
    mc_algorithms_surfaceNet
    mc_common
    )

add_subdirectory("./algorithms")
add_subdirectory("./common")
