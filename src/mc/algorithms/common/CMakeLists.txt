add_executable(generate_cube_tables
    generate_cube_tables.c
    )
foreach(table_file cube_tables.c canonical_cube_orientations.h)
  if(DEFINED ENV{EMSCRIPTEN})
    add_custom_target(${table_file}
        COMMAND node generate_cube_tables.js ${table_file} > ${table_file}
        )
  else()
    add_custom_target(${table_file}
        COMMAND generate_cube_tables ${table_file} > ${table_file}
        )
  endif()
  add_dependencies(${table_file} generate_cube_tables)
endforeach(table_file)

add_library(mc_algorithms_common STATIC
    cube.c
    dual.c
    surfaceNet.c
    )
add_dependencies(mc_algorithms_common
    canonical_cube_orientations.h
    cube_tables.c
    )
target_include_directories(mc_algorithms_common
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
