add_library(mc
    isosurfaceBuilder.c
    tables/common.c
    )
add_dependencies(mc simple.h)

add_subdirectory("./tables")
