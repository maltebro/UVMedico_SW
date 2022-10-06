file(GLOB SOURCES *.c)
list(APPEND EXTRA_COMPONENT_DIRS "./main")

idf_component_register(SRCS ${SOURCES}
                       INCLUDE_DIRS "."
                       REQUIRES "main")
                       
