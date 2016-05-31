if(DEFINED ENV{EMSCRIPTEN})
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]' -s DISABLE_EXCEPTION_CATCHING=0")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O2 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='[\"png\"]' -s ASSERTIONS=1 -s ALLOW_MEMORY_GROWTH=1")

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
endif()

add_subdirectory("./common")

add_executable(implicit
    implicit.cpp
    )

add_subdirectory("./cubes")
add_subdirectory("./torus")
