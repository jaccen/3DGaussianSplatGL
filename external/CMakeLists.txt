message(STATUS "> Found external directory")

set(CMAKE_FOLDER externals)

# glew configuration	
# checks if git submodule is already downloaded
if(CLONE_GLEW)
	if ((NOT EXISTS glew) OR 
		(NOT IS_DIRECTORY glew))
		message(STATUS "> Cloning glew from remote...")
		execute_process(
			COMMAND git submodule update --init glew
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	else()
		message(STATUS "> Found glew submodule folder")
	endif()
	add_subdirectory(glew)
endif()

if(CLONE_GLFW)
	# glfw configuration	
	# checks if git submodule is already downloaded
	if ((NOT EXISTS glfw) OR 
		(NOT IS_DIRECTORY glfw))
		message(STATUS "> Cloning GLFW from remote...")
		execute_process(
			COMMAND git submodule update --init glfw
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	else()
		message(STATUS "> Found GLFW submodule folder")
	endif()

	# OpenGL configuration
	# search for OpenGL on the system
	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIRS})
	set(GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only")
	set(GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only")

	# add glfw subdirectory after having found OpenGL
	add_subdirectory(glfw)

	if(MSVC)
		SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup" )
	endif()
endif()

if(CLONE_GLM)
	# glm configuration	
	# checks if git submodule is already downloaded
	if ((NOT EXISTS glm) OR 
		(NOT IS_DIRECTORY glm))
		message(STATUS "> Cloning glm from remote...")
		execute_process(
			COMMAND git submodule update --init glm
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	else()
		message(STATUS "> Found glm submodule folder")
	endif()
	add_subdirectory(glm)
endif()

if(CLONE_EIGEN)
	# Eigen3 configuration
	if ((NOT EXISTS eigen) OR 
		(NOT IS_DIRECTORY eigen))
		message(STATUS "> Cloning Eigen from remote...")
		execute_process(
			COMMAND git submodule update --init eigen
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
	else()
		message(STATUS "> Found Eigen3 submodule folder")
	endif()
	add_subdirectory(eigen)
endif()

if(CLONE_IMGUI)
	# Imgui configuration
	if ((NOT EXISTS imgui) OR 
		(NOT IS_DIRECTORY imgui))
		message(STATUS "> Cloning imgui from remote...")
		execute_process(
			COMMAND git submodule update --init imgui
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		)
	else()
		message(STATUS "> Found Imgui submodule folder")
	endif()

	set(target_name externals)
	set(imgui_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/imgui
		CACHE PATH "Imgui include directory")
	set(imgui_DIR ${CMAKE_SOURCE_DIR}/external/imgui
		CACHE PATH "Imgui directory")
		
	# setting up the target
	add_library(${target_name} STATIC "")
	target_link_libraries(${target_name} ${OPENGL_LIBRARIES} glfw Eigen3::Eigen)

	target_sources(${target_name}
		# cpp
		PRIVATE
			# GLFW
			"${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp"
			# OpenGL3
			"${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp"

			# sources
			"${PROJECT_SOURCE_DIR}/external/imgui/imgui.cpp"
			"${PROJECT_SOURCE_DIR}/external/imgui/imgui_demo.cpp"
			"${PROJECT_SOURCE_DIR}/external/imgui/imgui_draw.cpp"
			"${PROJECT_SOURCE_DIR}/external/imgui/imgui_tables.cpp"
			"${PROJECT_SOURCE_DIR}/external/imgui/imgui_widgets.cpp"

		# headers
		PRIVATE
			# GLFW
			"${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.h"
			# OpenGL3
			"${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.h"
			"${PROJECT_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3_loader.h"

			# sources
			"${PROJECT_SOURCE_DIR}/external/imgui/imconfig.h"
			"${PROJECT_SOURCE_DIR}/external/imgui/imgui.h"
			"${PROJECT_SOURCE_DIR}/external/imgui/imgui_internal.h"
			"${PROJECT_SOURCE_DIR}/external/imgui/imstb_rectpack.h"
			"${PROJECT_SOURCE_DIR}/external/imgui/imstb_textedit.h"
			"${PROJECT_SOURCE_DIR}/external/imgui/imstb_truetype.h"
		)

	target_include_directories(${target_name}
		PUBLIC
			"${PROJECT_SOURCE_DIR}/external/imgui"
			"${PROJECT_SOURCE_DIR}/external/imgui/backends"
			"${PROJECT_SOURCE_DIR}/external/glew/include"
			"${PROJECT_SOURCE_DIR}/external/glew"
			"${PROJECT_SOURCE_DIR}/external/glm"
			"${PROJECT_BINARY_DIR}"
	)

	# install the headers to make them available for the installed target
	install(DIRECTORY ${imgui_INCLUDE_DIR}/imgui DESTINATION ${CMAKE_BINARY_DIR}/external/imgui)

	# export the target so it can be included using find_package
	install(TARGETS externals EXPORT imgui DESTINATION ${CMAKE_BINARY_DIR}/external/imgui)
endif()
