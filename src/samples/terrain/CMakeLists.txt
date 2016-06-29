add_executable(terrain
    lodTree.cpp
    main.cpp
    terrain.cpp
    terrainGenerator.cpp
    terrainMesh.cpp
    )
target_link_libraries(terrain
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
  set_target_properties(terrain PROPERTIES
      SUFFIX ".html"
      COMPILE_FLAGS "${EMSCRIPTEN_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_FLAGS}"
      )
else()
  set_property(TARGET terrain PROPERTY CXX_STANDARD 11)
  set_property(TARGET terrain PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
