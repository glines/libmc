add_library(table_generation
    common.c
    )

add_executable(generate_simple
    generate_simple.c
    )
target_link_libraries(generate_simple
    table_generation
    )

add_custom_target(simple.h
    COMMAND generate_simple > simple.h
    )
