add_executable(adaptiveSquares
    main.cpp
    adaptiveShape.cpp
    )
target_link_libraries(adaptiveSquares
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
      "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
      "--preload-file ${CMAKE_CURRENT_SOURCE_DIR}/../common/assets/shaders@/assets/shaders"
     )
  string (REPLACE ";" " " EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}")
  set_target_properties(adaptiveSquares PROPERTIES
      SUFFIX ".html"
      COMPILE_FLAGS "${EMSCRIPTEN_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_FLAGS} ${EMSCRIPTEN_LINK_FLAGS}"
      )
else()
  set_property(TARGET adaptiveSquares PROPERTY CXX_STANDARD 11)
  set_property(TARGET adaptiveSquares PROPERTY CXX_STANDARD_REQUIRED ON)
endif()

