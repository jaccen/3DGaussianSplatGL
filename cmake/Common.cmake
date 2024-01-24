function(assert_outsource_build)
    if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
        message(FATAL_ERROR "
            Prevented in-source build. 
            Please create and use a build directory outside of the source code.")
    endif()
endfunction()

function(assign_source_group ROOT)
    foreach(_FILE IN ITEMS ${ARGN})
        file(RELATIVE_PATH _REL_PATH "${ROOT}" "${_FILE}")
        get_filename_component(_DIR "${_REL_PATH}" DIRECTORY)
        if(MSVC)
            string(REPLACE "/" "\\" _DIR "${_DIR}")
        endif()
        source_group("${_DIR}" FILES "${_FILE}")
    endforeach()
endfunction()

function(target_copy_files TARGET_NAME SOURCE_DIR DEST_DIR)
    file(GLOB SOURCE_FILES "${CMAKE_SOURCE_DIR}/${SOURCE_DIR}/*.*")
    foreach(_SOURCE_FILE IN ITEMS ${SOURCE_FILES})
        #file(COPY ${_SOURCE_FILE} DESTINATION ${DEST_DIR})
        cmake_path(GET _SOURCE_FILE FILENAME _FILENAME)
        set(_DEST_FILE "${CMAKE_CURRENT_BINARY_DIR}/${DEST_DIR}/${_FILENAME}")
        add_custom_command(
            OUTPUT ${_DEST_FILE}
            COMMAND ${CMAKE_COMMAND} -E copy ${_SOURCE_FILE} ${_DEST_FILE}
            VERBATIM)
        target_sources(${TARGET_NAME} PRIVATE ${_DEST_FILE})
    endforeach()
endfunction()

function(set_high_warning_level TARGET)
    if(MSVC)
        target_compile_options(${TARGET} PRIVATE /W4 /WX)
    else()
        target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endfunction()

include(FetchContent)

function(fetch_gtest)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.12.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    set_target_properties(gmock PROPERTIES FOLDER "external/googletest")
    set_target_properties(gmock_main PROPERTIES FOLDER "external/googletest")
    set_target_properties(gtest PROPERTIES FOLDER "external/googletest")
    set_target_properties(gtest_main PROPERTIES FOLDER "external/googletest")
endfunction()

function(fetch_glm)
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 0.9.9.8
    )
    FetchContent_MakeAvailable(glm)
endfunction()

function(fetch_glfw)
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.3.8
    )
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(glfw)
    set_target_properties(glfw PROPERTIES FOLDER "external/glfw3")
    set_target_properties(update_mappings PROPERTIES FOLDER "external/glfw3")
endfunction()