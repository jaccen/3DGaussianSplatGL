cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

project(tinyEngine)
add_subdirectory(lib)
# The main executable
add_executable(gaussian
  src/Application.cpp
  src/Application.cpp
  src/MyApplication.cpp
  src/MyApplication.cpp
  src/glError.hpp
  src/glError.cpp
  src/main.cpp
  src/Shader.hpp
  src/Shader.cpp
  src/texture.h
  src/texture.cpp
)

set_property(TARGET gaussian PROPERTY CXX_STANDARD 17)
target_compile_options(gaussian PRIVATE -Wall)

add_definitions(-DGLEW_STATIC)
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)
add_subdirectory(lib/glew EXCLUDE_FROM_ALL)
add_subdirectory(lib/glm EXCLUDE_FROM_ALL)
add_subdirectory(lib/stb EXCLUDE_FROM_ALL)

set(STB_DIR "${CMAKE_SOURCE_DIR}/lib/stb")
set(STB_SRC "${STB_DIR}/stb_image.h")
assign_source_group(${STB_DIR} ${STB_SRC})

target_link_libraries(gaussian
  PRIVATE glfw
  PRIVATE libglew_static
  PRIVATE glm
  PRIVATE stb
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/asset.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/asset.hpp
)

target_include_directories(gaussian
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
  PRIVATE ${STB_DIR}
)

