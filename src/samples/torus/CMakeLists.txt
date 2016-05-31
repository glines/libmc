add_executable(torus
    main.c
    )
target_link_libraries(torus
    ${GLEW_LIBRARY}
    ${SDL2_LIBRARY}
    m
    mc
    )
if(DEFINED ENV{EMSCRIPTEN})
  # TODO: Use preloading instead
  set_target_properties(torus PROPERTIES
      SUFFIX ".html"
      LINK_FLAGS "--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0")
endif()
