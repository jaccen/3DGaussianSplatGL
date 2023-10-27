# Projects Settings
cmake_minimum_required (VERSION 3.1)
project (3DGaussianSplatGL)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
# The main executable
add_executable(3DGaussianSplatGL
  src/Application.cpp
  src/Application.cpp
  src/MyApplication.cpp
  src/MyApplication.cpp
  src/glError.hpp
  src/glError.cpp
  src/main.cpp
  src/Shader.hpp
  src/Shader.cpp
  src/dataset.cpp
  src/gui.cpp
  src/logging.cpp
  src/ply.cpp
  src/Texture.cpp
)

set_property(TARGET 3DGaussianSplatGL PROPERTY CXX_STANDARD 20)
target_compile_options(3DGaussianSplatGL PRIVATE -Wall)

add_definitions(-DGLEW_STATIC)
add_subdirectory(external/glfw EXCLUDE_FROM_ALL)
add_subdirectory(external/glew EXCLUDE_FROM_ALL)
add_subdirectory(external/glm EXCLUDE_FROM_ALL)
#add_subdirectory(external/imgui EXCLUDE_FROM_ALL)
add_subdirectory(external/cxxopts EXCLUDE_FROM_ALL)
#add_subdirectory(external/llfio EXCLUDE_FROM_ALL)
add_subdirectory(external/eigen EXCLUDE_FROM_ALL)
add_subdirectory(external/tinyply EXCLUDE_FROM_ALL)


target_link_libraries(3DGaussianSplatGL
  PRIVATE glfw
  PRIVATE libglew_static
  PRIVATE glm
  #PRIVATE imgui
  PRIVATE cxxopts
  #PRIVATE llfio
  PRIVATE eigen
  PRIVATE tinyply

)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/asset.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/src/asset.hpp
)
target_include_directories(3DGaussianSplatGL
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
  PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/external

)
