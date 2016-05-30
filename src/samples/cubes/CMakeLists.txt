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
set_property(TARGET cubes PROPERTY CXX_STANDARD 11)
set_property(TARGET cubes PROPERTY CXX_STANDARD_REQUIRED ON)
