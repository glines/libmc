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
      "--shell-file ${CMAKE_CURRENT_SOURCE_DIR}/assets/shell/shell_minimal.html"
      "-s EXPORTED_FUNCTIONS='[\"_main\",\"_update_code\"]'"
     )
  string (REPLACE ";" " " EMSCRIPTEN_LINK_FLAGS "${EMSCRIPTEN_LINK_FLAGS}")
  set_target_properties(implicit PROPERTIES
      SUFFIX ".html"
      COMPILE_FLAGS "${EMSCRIPTEN_FLAGS}"
      LINK_FLAGS "${EMSCRIPTEN_FLAGS} ${EMSCRIPTEN_LINK_FLAGS}"
      )
  # The CodeMirror editor is provided for editing Lua and Javascript code from
  # within the browser
  add_custom_command(TARGET implicit POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
      "${CMAKE_SOURCE_DIR}/extern/codemirror-5.15.2/lib"
      "${CMAKE_CURRENT_BINARY_DIR}/codemirror")
  add_custom_command(TARGET implicit POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
      "${CMAKE_SOURCE_DIR}/extern/codemirror-5.15.2/mode/lua/lua.js"
      "${CMAKE_CURRENT_BINARY_DIR}/codemirror/lua.js")
else()
  set_property(TARGET implicit PROPERTY CXX_STANDARD 11)
  set_property(TARGET implicit PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
