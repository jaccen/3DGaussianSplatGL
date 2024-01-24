add_library(Glad)

set(GLAD_DIR "${CMAKE_SOURCE_DIR}/external/glad")
set(GLAD_SRC 
    "${GLAD_DIR}/src/glad.c"
    "${GLAD_DIR}/include/glad/glad.h")
assign_source_group(${GLAD_DIR} ${GLAD_SRC})

target_sources(Glad PRIVATE ${GLAD_SRC})
target_include_directories(Glad PUBLIC "${GLAD_DIR}/include")

set_target_properties(Glad PROPERTIES FOLDER external)