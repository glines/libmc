# GLM Mathematics Library
find_package(GLM REQUIRED)

# GLEW library
find_package(GLEW REQUIRED)

# SDL2 library
find_package(SDL2 REQUIRED)

include_directories(SYSTEM
    ${GLEW_INCLUDE_DIR}
    ${GLM_INCLUDE_DIR}
    ${SDL2_INCLUDE_DIR}
    )

add_subdirectory("./common")

add_executable(implicit
    implicit.cpp
    )

add_subdirectory("./cubes")
