add_library(mcxx
    isosurfaceBuilder.cpp
    mesh.cpp
    scalarField.cpp
    )
target_link_libraries(mcxx
    mc
    )
set_property(TARGET mcxx PROPERTY CXX_STANDARD 11)
set_property(TARGET mcxx PROPERTY CXX_STANDARD_REQUIRED ON)
