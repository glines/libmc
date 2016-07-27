add_executable(transition
    interpolatingLattice.cpp
    interpolatingOctree.cpp
    main.cpp
    transitionObject.cpp
    transitionOctree.cpp
    )
target_link_libraries(transition
    ${GLEW_LIBRARY}
    ${SDL2_LIBRARY}
    mcxx
    samples_common
    )

if(DEFINED ENV{EMSCRIPTEN})
  set(EMSCRIPTEN_FLAGS
      "-s USE_SDL=2"
     )
  string (REPLACE ";" " " EMSCRIPTEN_FLAGS "${EMSCRIPTEN_FLAGS}")
  set(EMSCRIPTEN_LINK_FLAGS
      "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/../common/assets/shaders@/assets/shaders"
     )
  string (REPLACE ";" " " EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}")
  set_target_properties(transition PROPERTIES
      SUFFIX ".html"
      COMPILE_FLAGS "${EMSCRIPTEN_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_FLAGS} ${EMSCRIPTEN_LINK_FLAGS}"
      )
else()
  set_property(TARGET transition PROPERTY CXX_STANDARD 11)
  set_property(TARGET transition PROPERTY CXX_STANDARD_REQUIRED ON)
endif()


# Take a screenshot of each of the transition cell configurations
if(BUILD_SCREENSHOTS AND NOT DEFINED ENV{EMSCRIPTEN})
  foreach(i RANGE 0 511)
    execute_process(
        COMMAND printf cell=0x%02x,camera=topLeft ${i}
        OUTPUT_VARIABLE scene
        )
    execute_process(
        COMMAND printf MC_TRANSVOXEL_transition_cell_0x%03x.png ${i}
        OUTPUT_VARIABLE screenshot
        )
    add_custom_target(${screenshot} ALL
        COMMAND transition
          --scene "${scene}"
          --width 1500 --height 1500
          --screenshot - > "${CMAKE_BINARY_DIR}/${screenshot}"
        )
  endforeach(i)
endif()
