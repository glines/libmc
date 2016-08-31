add_executable(colored_marching_squares_generate_tables
    common.c
    generate_tables.c
    )
target_link_libraries(colored_marching_squares_generate_tables
    mc_algorithms_marchingSquares
    )
generate_files(colored_marching_squares_generate_tables
    colored_marching_squares_tables.c
    colored_marching_squares_canonical.h
    )

add_executable(colored_marching_squares_generate_line_tables
    canonical.c
    common.c
    generate_line_tables.c
    )
target_link_libraries(colored_marching_squares_generate_line_tables
    mc_algorithms_common
    )
add_dependencies(colored_marching_squares_generate_line_tables
    colored_marching_squares_canonical.h
    )
target_include_directories(colored_marching_squares_generate_line_tables
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
generate_files(colored_marching_squares_generate_line_tables
    colored_marching_squares_line_tables.c
    )

add_library(mc_algorithms_coloredMarchingSquares
    coloredMarchingSquares.c
    )
add_dependencies(mc_algorithms_coloredMarchingSquares
    colored_marching_squares_line_tables.c
    colored_marching_squares_tables.c
    )
target_include_directories(mc_algorithms_coloredMarchingSquares
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
