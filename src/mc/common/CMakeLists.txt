add_library(mc_common STATIC
    contour.c
    mesh.c
    vector.c
    )
target_include_directories(mc_common
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
    )
