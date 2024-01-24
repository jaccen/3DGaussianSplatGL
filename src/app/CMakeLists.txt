add_executable(app)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")

assign_source_group(${CMAKE_CURRENT_SOURCE_DIR} ${SRC_FILES})

target_sources(app PRIVATE ${SRC_FILES})
target_include_directories(app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(app PRIVATE runtime)

target_copy_files(app "data" "data")
target_copy_files(app "src/runtime/render/shaders" "shaders")

if(EMSCRIPTEN)
    target_link_options(app PRIVATE 
        "-v --emrun"
        "-s DEMANGLE_SUPPORT=1"
        "-s USE_GLFW=3 -s USE_WEBGL2=1"
        "-s FULL_ES3=1"
        "-o Main.html")
endif()

set_high_warning_level(app)