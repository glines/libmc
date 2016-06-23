add_library(mc_algorithms_cuberille STATIC
    cuberille.c
    )
target_include_directories(mc_algorithms_cuberille SYSTEM
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
    )

# Take screenshots representitive of the Cuberille algorithm
take_screenshots("MC_CUBERILLE")
