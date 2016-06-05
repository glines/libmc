add_executable(generate_cube_tables
    cube_definitions.c
    generate_cube_tables.c
    )
if(DEFINED ENV{EMSCRIPTEN})
  add_custom_target(cube_tables.h
      COMMAND node generate_cube_tables.js > cube_tables.h
      )
else()
  add_custom_target(cube_tables.h
      COMMAND generate_cube_tables > cube_tables.h
      )
endif()
add_dependencies(cube_tables.h generate_cube_tables)

# TODO: Move the routines from mc_algorithms_simple to this library
add_library(mc_algorithms_common STATIC
    cube.c
    cube_definitions.c
    surfaceNet.c
    )
add_dependencies(mc_algorithms_common cube_tables.h)
target_include_directories(mc_algorithms_common SYSTEM
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
    )
target_include_directories(mc_algorithms_common
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
