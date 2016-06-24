function(take_screenshots algorithm)
  if(NOT DEFINED ENV{EMSCRIPTEN})
    # Take screenshots representitive of the given algorithm
    add_custom_target(${algorithm}_background.png ALL
        COMMAND screenshots
          --algorithm ${algorithm}
          --screenshot - > "${CMAKE_BINARY_DIR}/${algorithm}_background.png"
        )
    add_dependencies(${algorithm}_background.png
        screenshots
        )
  endif()
endfunction(take_screenshots)

add_subdirectory("./common")

add_subdirectory("./cuberille")
add_subdirectory("./cubes33")
add_subdirectory("./delaunay")
add_subdirectory("./dualMarchingCubes")
add_subdirectory("./elasticSurfaceNet")
add_subdirectory("./nielsonDual")
add_subdirectory("./patch")
add_subdirectory("./simple")
add_subdirectory("./snapmc")
add_subdirectory("./surfaceNet")
