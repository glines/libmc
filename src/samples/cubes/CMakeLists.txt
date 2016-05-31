add_executable(cubes
    cubeObject.cpp
    main.cpp
    )
target_link_libraries(cubes
    ${GLEW_LIBRARY}
    ${SDL2_LIBRARY}
    mcxx
    samples_common
    )

if(DEFINED ENV{EMSCRIPTEN})
  # TODO: Use preloading instead
  set_target_properties(cubes PROPERTIES
      SUFFIX ".html"
      LINK_FLAGS "--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0")
  target_compile_options(cubes PRIVATE -std=c++11)
else()
  set_property(TARGET cubes PROPERTY CXX_STANDARD 11)
  set_property(TARGET cubes PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
