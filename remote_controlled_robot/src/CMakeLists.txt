idf_component_register(
    SRCS 
        "main.c"
        "pump.c"
        "blower.c"
    INCLUDE_DIRS "."
    REQUIRES servo
)