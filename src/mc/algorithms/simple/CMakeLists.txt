# Generate the simple marching cubes table header file
add_executable(generate_simple_table
    generate_table.c
    )
target_include_directories(generate_simple_table
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
target_link_libraries(generate_simple_table
    mc_algorithms_common
    )
target_include_directories(generate_simple_table SYSTEM
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
    )
if(DEFINED ENV{EMSCRIPTEN})
  add_custom_target(simple_table.h
      COMMAND node generate_simple_table.js > simple_table.h
      )
else()
  add_custom_target(simple_table.h
      COMMAND generate_simple_table > simple_table.h
      )
endif()
add_dependencies(simple_table.h generate_simple_table)

# Build the algorithm in a static library, including the table header
add_library(mc_algorithms_simple STATIC
    simple.c
    )
target_link_libraries(mc_algorithms_simple
    mc_common
    )
add_dependencies(mc_algorithms_simple simple_table.h)
target_include_directories(mc_algorithms_simple SYSTEM
    PRIVATE "${CMAKE_SOURCE_DIR}/include"
    )
target_include_directories(mc_algorithms_simple
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}"
    )
