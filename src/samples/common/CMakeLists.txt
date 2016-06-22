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
target_link_libraries(samples_common
    ${LUA_LIBRARIES}
    )

if(DEFINED ENV{EMSCRIPTEN})
  target_compile_options(samples_common PRIVATE -std=c++11)
else()
  set_property(TARGET samples_common PROPERTY CXX_STANDARD 11)
  set_property(TARGET samples_common PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
