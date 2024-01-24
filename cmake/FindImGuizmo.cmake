add_library(ImGuizmo)

set(IMGUIZMO_DIR "${CMAKE_SOURCE_DIR}/external/imguizmo/1.83")
set(IMGUIZMO_SRC "${IMGUIZMO_DIR}/ImGuizmo.cpp")
assign_source_group(${IMGUIZMO_DIR} ${IMGUIZMO_SRC})

target_sources(ImGuizmo PRIVATE ${IMGUIZMO_SRC})
target_include_directories(ImGuizmo PUBLIC ${IMGUIZMO_DIR})
target_link_libraries(ImGuizmo PRIVATE ImGui)

set_target_properties(ImGuizmo PROPERTIES FOLDER external)