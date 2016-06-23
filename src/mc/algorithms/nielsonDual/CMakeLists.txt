# Generate the tables for the Nielson MC-Dual algorithm
add_executable(generate_nielsonDual_tables
    generate_nielsonDual_tables.c
    )
add_dependencies(generate_nielsonDual_tables
    canonical_cube_orientations.h
    )
target_include_directories(generate_nielsonDual_tables
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
target_link_libraries(generate_nielsonDual_tables
    m
    mc_algorithms_common
    mc_common
    )
if(DEFINED ENV{EMSCRIPTEN})
  add_custom_target(nielsonDual_tables.c
      COMMAND node generate_nielsonDual_tables.js > nielsonDual_tables.c
      )
else()
  add_custom_target(nielsonDual_tables.c
      COMMAND generate_nielsonDual_tables > nielsonDual_tables.c
      )
endif()
add_dependencies(nielsonDual_tables.c
    generate_nielsonDual_tables
    )

# Build the library for the Nielson MC-Dual algorithm
add_library(mc_algorithms_nielsonDual STATIC
    nielsonDual.c
    )
add_dependencies(mc_algorithms_nielsonDual
    nielsonDual_tables.c
    )
target_include_directories(mc_algorithms_nielsonDual
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )

# Take screenshots representitive of the Nielson MC-Dual algorithm
take_screenshots("MC_NIELSON_DUAL")
