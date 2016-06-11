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
  set_target_properties(scan PROPERTIES
      SUFFIX ".html"
      LINK_FLAGS "--embed-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@/assets -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1 -s DISABLE_EXCEPTION_CATCHING=0")
  target_compile_options(scan PRIVATE -std=c++11)
else()
  set_property(TARGET scan PROPERTY CXX_STANDARD 11)
  set_property(TARGET scan PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
