add_executable(scan
    main.cpp
    scan.cpp
    scanObject.cpp
    )
target_link_libraries(scan
    ${GLEW_LIBRARY}
    ${PNG_LIBRARIES}
    ${SDL2_LIBRARY}
    mcxx
    samples_common
    )

if(DEFINED ENV{EMSCRIPTEN})
  # TODO: Use preloading instead
  set(EMSCRIPTEN_FLAGS
      "-s USE_SDL=2"
      "-s USE_LIBPNG=1"
     )
  string (REPLACE ";" " " EMSCRIPTEN_FLAGS "${EMSCRIPTEN_FLAGS}")
  set_target_properties(scan PROPERTIES
      SUFFIX ".html"
      COMPILE_FLAGS "${EMSCRIPTEN_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_FLAGS} --embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets"
      )
else()
  set_property(TARGET scan PROPERTY CXX_STANDARD 11)
  set_property(TARGET scan PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
