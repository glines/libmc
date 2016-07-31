enable_testing()

add_executable(cube_test
    cube.c
    )
target_link_libraries(cube_test
    mc
    )
add_test(cube_test cube_test)
