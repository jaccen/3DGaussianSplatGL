add_library(Stb INTERFACE)

set(STB_DIR "${CMAKE_SOURCE_DIR}/external/stb")
set(STB_SRC "${STB_DIR}/stb_image.h")
assign_source_group(${STB_DIR} ${STB_SRC})

target_include_directories(Stb INTERFACE ${STB_DIR})

set_target_properties(Stb PROPERTIES FOLDER external)