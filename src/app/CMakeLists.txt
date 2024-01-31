add_executable(app)
# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 20 CACHE STRING "v")
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #Optional
#关闭将警告视为错误
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W0")



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