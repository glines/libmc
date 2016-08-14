add_executable(marching_squares_generate_tables
    generate_tables.c
    )
target_link_libraries(marching_squares_generate_tables
    mc_algorithms_common
    )
generate_files(marching_squares_generate_tables
    marching_squares_canonical_squares.h
    marching_squares_tables.c
    )

add_executable(marching_squares_generate_line_tables
    canonical.c
    generate_line_tables.c
    )
target_link_libraries(marching_squares_generate_line_tables
    mc_algorithms_common
    )
add_dependencies(marching_squares_generate_line_tables
    marching_squares_canonical_squares.h
    marching_squares_tables.c
    )
target_include_directories(marching_squares_generate_line_tables
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
generate_files(marching_squares_generate_line_tables
    marching_squares_line_tables.c
    )

add_library(mc_algorithms_marchingSquares
    canonical.c
    marchingSquares.c
    )
target_link_libraries(mc_algorithms_marchingSquares
    mc_algorithms_common
    )
add_dependencies(mc_algorithms_marchingSquares
    marching_squares_line_tables.c
    marching_squares_tables.c
    )
target_include_directories(mc_algorithms_marchingSquares
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
