add_library(samples_common STATIC
    arcballCamera.cpp
    camera.cpp
    glError.cpp
    perspectiveCamera.cpp
    scene.cpp
    sceneObject.cpp
    shaderProgram.cpp
    transform.cpp
    transformRAII.cpp
    transformStack.cpp
    )

set_property(TARGET samples_common PROPERTY CXX_STANDARD 11)
set_property(TARGET samples_common PROPERTY CXX_STANDARD_REQUIRED ON)
