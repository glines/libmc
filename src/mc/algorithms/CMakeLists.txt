function(take_screenshots algorithm)
  if(BUILD_SCREENSHOTS AND NOT DEFINED ENV{EMSCRIPTEN})
    # Take screenshots representitive of the given algorithm
    foreach(i RANGE 0 255)
      execute_process(
          COMMAND printf algorithm=%s,cube=0x%02x,camera=topLeft ${algorithm} ${i}
          OUTPUT_VARIABLE scene
          )
      execute_process(
          COMMAND printf %s_cube_%02x.png ${algorithm} ${i}
          OUTPUT_VARIABLE screenshot
          )
      add_custom_target(${screenshot} ALL
          COMMAND cubes
            --scene "${scene}"
            --screenshot - > "${CMAKE_BINARY_DIR}/${screenshot}"
          )
      add_dependencies(${screenshot}
          cubes
          )
    endforeach(i)
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
