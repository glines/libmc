add_executable(transvoxel
    frustum.cpp
    main.cpp
    transvoxelTerrain.cpp
    transvoxelTree.cpp
    )
target_link_libraries(transvoxel
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
  set_target_properties(transvoxel PROPERTIES
      SUFFIX ".html"
      COMPILE_FLAGS "${EMSCRIPTEN_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_FLAGS}"
      )
else()
  set_property(TARGET transvoxel PROPERTY CXX_STANDARD 11)
  set_property(TARGET transvoxel PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
