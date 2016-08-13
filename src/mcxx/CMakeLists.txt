add_library(mcxx
    contour.cpp
    contourBuilder.cpp
    isosurfaceBuilder.cpp
    mesh.cpp
    scalarField.cpp
    vector.cpp
    )
target_link_libraries(mcxx
    mc
    )
if(DEFINED ENV{EMSCRIPTEN})
  target_compile_options(mcxx PRIVATE -std=c++11)
else()
  set_property(TARGET mcxx PROPERTY CXX_STANDARD 11)
  set_property(TARGET mcxx PROPERTY CXX_STANDARD_REQUIRED ON)
endif()
