# Generate the tables used to transform cell configurations
add_executable(transvoxel_generate_transform_tables
    generate_transform_tables.c
    )
generate_files(transvoxel_generate_transform_tables
    transvoxel_transform_tables.c 
    )

# Generate the tables with all canonical cube and cell configurations
add_executable(transvoxel_generate_canonical_tables
    generate_canonical_tables.c
    transform.c
    )
add_dependencies(transvoxel_generate_canonical_tables
    transvoxel_transform_tables.c
    )
target_include_directories(transvoxel_generate_canonical_tables
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
generate_files(transvoxel_generate_canonical_tables
    transvoxel_canonical_cell_tables.c
    transvoxel_canonical_cells.h
    )

# Generate the cube and cell edge intersection tables
add_executable(transvoxel_generate_edge_tables
    generate_edge_tables.c
    )
generate_files(transvoxel_generate_edge_tables
    transvoxel_edge_tables.c
    )

# Generate the cube and cell triangulation tables
add_executable(transvoxel_generate_triangulation_tables
    canonical.c
    generate_triangulation_tables.c
    transform.c
    )
add_dependencies(transvoxel_generate_triangulation_tables
    transvoxel_canonical_cell_tables.c
    transvoxel_canonical_cells.h
    transvoxel_transform_tables.c
    )
target_include_directories(transvoxel_generate_triangulation_tables
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
generate_files(transvoxel_generate_triangulation_tables
    transvoxel_triangulation_tables.c
    )

add_library(mc_algorithms_transvoxel STATIC
    canonical.c
    edges.c
    transform.c
    transvoxel.c
    triangles.c
    )
add_dependencies(mc_algorithms_transvoxel
    transvoxel_canonical_cell_tables.c
    transvoxel_canonical_cells.h
    transvoxel_edge_tables.c
    transvoxel_triangulation_tables.c
    )
target_include_directories(mc_algorithms_transvoxel
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
