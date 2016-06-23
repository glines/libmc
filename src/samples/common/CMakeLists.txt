add_library(samples_common STATIC
    arcballCamera.cpp
    camera.cpp
    glError.cpp
    meshObject.cpp
    orthographicCamera.cpp
    perspectiveCamera.cpp
    scene.cpp
    sceneObject.cpp
    shaderProgram.cpp
    template.cpp
    transform.cpp
    transformRAII.cpp
    transformStack.cpp
    )
target_include_directories(samples_common
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}"
    )
target_link_libraries(samples_common
    ${LUA_LIBRARIES}
    )

if(DEFINED ENV{EMSCRIPTEN})
  target_compile_options(samples_common PRIVATE -std=c++11)
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
