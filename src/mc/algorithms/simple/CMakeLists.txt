# TODO: rename generate_table.c to generate_simple_tables.c
# TODO: rename generate_simple_table to generate_simple_tables
# Generate the simple marching cubes table header file
add_executable(generate_simple_table
    generate_table.c
    )
add_dependencies(generate_simple_table canonical_cube_orientations.h)
target_include_directories(generate_simple_table
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
target_link_libraries(generate_simple_table
    mc_algorithms_common
    )
if(DEFINED ENV{EMSCRIPTEN})
  add_custom_target(simple_tables.c
      COMMAND node generate_simple_table.js > simple_tables.c
      )
else()
  add_custom_target(simple_tables.c
      COMMAND generate_simple_table > simple_tables.c
      )
endif()
add_dependencies(simple_tables.c generate_simple_table)

# Build the algorithm in a static library, including the table header
add_library(mc_algorithms_simple STATIC
    simple.c
    )
target_link_libraries(mc_algorithms_simple
    mc_common
    )
add_dependencies(mc_algorithms_simple simple_tables.c)
target_include_directories(mc_algorithms_simple
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )

# Take screenshots representitive of the simple marching cubes algorithm
take_screenshots("MC_SIMPLE_MARCHING_CUBES")
