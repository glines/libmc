add_subdirectory("./mc")
add_subdirectory("./mcxx")
add_subdirectory("./tests")

if(BUILD_SAMPLES)
  add_subdirectory("./samples")
endif()
