add_library(runtime STATIC)

find_package(Glad REQUIRED)
find_package(ImGui REQUIRED)
find_package(ImGuizmo REQUIRED)
find_package(Stb REQUIRED)
find_package(OpenGL REQUIRED)

fetch_glm()
fetch_glfw()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
file(GLOB_RECURSE SRC_SHADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/render/shaders/*.frag"
    "${CMAKE_CURRENT_SOURCE_DIR}/render/shaders/*.vert")

assign_source_group(${CMAKE_CURRENT_SOURCE_DIR} ${SRC_FILES})
assign_source_group(${CMAKE_CURRENT_SOURCE_DIR} ${SRC_FILES})

target_sources(runtime PRIVATE ${SRC_FILES} ${SRC_SHADER_FILES})
target_include_directories(runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(runtime PUBLIC
    NOMINMAX
    IMGUI_USER_CONFIG=<ui/ImConfig.hpp>)
target_link_libraries(runtime
    PUBLIC
        Glad
        glm
        glfw
        ImGui
        ImGuizmo
        Stb
        OpenGL::GL)

set_high_warning_level(runtime)