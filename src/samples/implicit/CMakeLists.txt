add_executable(implicit
    implicitSurface.cpp
    main.cpp
    )
target_link_libraries(implicit
    ${GLEW_LIBRARY}
    ${LUA_LIBRARIES}
    ${SDL2_LIBRARY}
    mcxx
    samples_common
    )
message("LUA_LIBRARIES: ${LUA_LIBRARIES}")

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
  set_target_properties(implicit PROPERTIES
      SUFFIX ".html"
      COMPILE_FLAGS "${EMSCRIPTEN_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_FLAGS} ${EMSCRIPTEN_LINK_FLAGS}"
      )
else()
  set_property(TARGET implicit PROPERTY CXX_STANDARD 11)
  set_property(TARGET implicit PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
