add_library(samples_common STATIC
    arcballCamera.cpp
    camera.cpp
    demo.cpp
    glError.cpp
    meshObject.cpp
    orthographicCamera.cpp
    perspectiveCamera.cpp
    scene.cpp
    sceneObject.cpp
    shaderProgram.cpp
    shaders.cpp
    template.cpp
    transform.cpp
    transformRAII.cpp
    transformStack.cpp
    wasdCamera.cpp
    )
target_include_directories(samples_common
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
target_link_libraries(samples_common
    ${LUA_LIBRARIES}
    ${PNG_LIBRARY}
    )

if(DEFINED ENV{EMSCRIPTEN})
  set(EMSCRIPTEN_FLAGS
      "-s USE_LIBPNG=1"
      "-s USE_SDL=2"
     )
  string (REPLACE ";" " " EMSCRIPTEN_FLAGS "${EMSCRIPTEN_FLAGS}")
  set_target_properties(samples_common PROPERTIES
      COMPILE_FLAGS "${EMSCRIPTEN_FLAGS}"
      )
else()
  set_property(TARGET samples_common PROPERTY CXX_STANDARD 11)
  set_property(TARGET samples_common PROPERTY CXX_STANDARD_REQUIRED ON)
endif()

# Convert all of our shaders to C header files
file(GLOB_RECURSE shaders
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/*.vert"
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/*.frag"
    )
foreach(shader ${shaders})
  file(RELATIVE_PATH shader_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${shader}")
  string(REPLACE "/" "_" shader_target "${shader_rel}.c")
  get_filename_component(shader_dir "${shader_rel}" DIRECTORY)
  add_custom_target("${shader_target}"
      COMMAND xxd -include ${shader_rel} > "${CMAKE_CURRENT_BINARY_DIR}/${shader_target}"
      DEPENDS "${shader}"
      WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
      )
  add_dependencies(samples_common
      "${shader_target}"
      )
endforeach()
