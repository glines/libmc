if(DEFINED ENV{EMSCRIPTEN})
  # Temporarily disable the default Emscripten behavior of ignoring system
  # include directories so that we can include the header-only GLM library
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)

  # GLM Mathematics Library
  find_package(GLM REQUIRED)

  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

  include_directories(SYSTEM
      ${GLM_INCLUDE_DIR}
      )
else()
  # GLEW library
  find_package(GLEW REQUIRED)

  # GLM Mathematics Library
  find_package(GLM REQUIRED)

  # PNG Image Library
  find_package(PNG REQUIRED)

  # SDL2 library
  find_package(SDL2 REQUIRED)

  include_directories(SYSTEM
      ${GLEW_INCLUDE_DIR}
      ${GLM_INCLUDE_DIR}
      ${PNG_INCLUDE_DIRS}
      ${SDL2_INCLUDE_DIR}
      )
endif()

add_subdirectory("./common")

add_executable(implicit
    implicit.cpp
    )

add_subdirectory("./cubes")
add_subdirectory("./scan")
add_subdirectory("./torus")
