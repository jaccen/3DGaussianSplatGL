#include <ui/ImGui.hpp>

#include <core/Core.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

namespace
{
    std::array<float, 16> unwrapMatrix(const jaccen::mat4& matrix)
    {
        const float* source = static_cast<const float*>(glm::value_ptr(matrix));

        std::array<float, 16> dest;
        for (int i = 0; i < 16; ++i)
        {
            dest[i] = source[i];
        }
        return dest;
    }
}

bool jaccen::imgui::gizmoManipulate(
    mat4& model, 
    const mat4& view,
    const mat4& projection,
    ImGuizmo::OPERATION operation,
    ImGuizmo::MODE mode)
{
    auto modelMatrix = unwrapMatrix(model);
    auto viewMatrix = unwrapMatrix(view);
    auto projectionMatrix = unwrapMatrix(projection);

    auto& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    
    // ImGuizmo calculates the model delta wrong for scale,
    // it gets the actual value and not the transition: 
    // https://github.com/CedricGuillemet/ImGuizmo/issues/29

    // Setup gizmo to be drawed
    ImGuizmo::Manipulate(
        &viewMatrix[0],
        &projectionMatrix[0],
        operation,
        mode,
        &modelMatrix[0],
        nullptr,
        nullptr,  // useSnap ? &snap[0]
        nullptr,  // boundSizing ? bounds
        nullptr); // boundSizingSnap ? boundsSnap 

    model = glm::make_mat4(&modelMatrix[0]);
    
    // Return whether gizmo was manipulated this frame or not
    return ImGuizmo::IsUsing();
}

//ImGuizmo::DrawCube(viewMatrix, projectionMatrix, modelMatrix);
//float translation[3] = { 0 };
//float rotation[3] = { 0 };
//float scale[3] = { 0 };
//ImGuizmo::DecomposeMatrixToComponents(modelMatrix, translation, rotation, scale);
//ImGui::InputFloat3("Tr", matrixTranslation, 3);
//ImGui::InputFloat3("Rt", matrixRotation, 3);
//ImGui::InputFloat3("Sc", matrixScale, 3);
//ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, modelMatrix);

