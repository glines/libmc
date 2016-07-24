add_executable(generate_patch_tables
    generate_patch_tables.c
    )
target_include_directories(generate_patch_tables
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
add_dependencies(generate_patch_tables canonical_cube_orientations.h)
target_link_libraries(generate_patch_tables
    m
    mc_algorithms_common
    mc_algorithms_simple
    )
if(DEFINED ENV{EMSCRIPTEN})
  add_custom_target(patch_tables.c
      COMMAND node generate_patch_tables.js > patch_tables.c
      )
else()
  add_custom_target(patch_tables.c
      COMMAND generate_patch_tables > patch_tables.c
      )
endif()
add_dependencies(patch_tables.c generate_patch_tables)

add_library(mc_algorithms_patch STATIC
    patch.c
    )
target_link_libraries(mc_algorithms_patch
    mc_algorithms_simple
    )
add_dependencies(mc_algorithms_patch patch_tables.c)
target_include_directories(mc_algorithms_patch
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )

# Take screenshots representitive of the patch marching cubes algorithm
take_screenshots("MC_PATCH_MARCHING_CUBES")
