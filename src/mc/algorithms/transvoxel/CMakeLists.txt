add_executable(generate_transvoxel_tables
    generate_transvoxel_tables.c
    )
if(DEFINED ENV{EMSCRIPTEN})
  add_custom_target(transvoxel_tables.c
      COMMAND node generate_transvoxel_tables.js > transvoxel_tables.c
      )
else()
  add_custom_target(transvoxel_tables.c
      COMMAND generate_transvoxel_tables > transvoxel_tables.c
      )
endif()
add_dependencies(transvoxel_tables.c generate_transvoxel_tables)

add_library(mc_algorithms_transvoxel STATIC
    transvoxel.c
    )
add_dependencies(mc_algorithms_transvoxel
    transvoxel_tables.c
    )
target_include_directories(mc_algorithms_transvoxel
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
