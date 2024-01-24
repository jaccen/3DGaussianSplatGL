#include <editor/EditorSystem.hpp>

#include <core/Core.hpp>
#include <render/Raycast.hpp>
#include <render/components/Mesh.hpp>
#include <scene/Scene.hpp>
#include <scene/components/Camera.hpp>
#include <ui/Window.hpp>

using namespace jaccen;

EditorSystem::EditorSystem(
    Database& db) :
    m_hoveredTable(db.createTable<Hovered>()),
    m_selectedTable(db.createTable<Selected>()),
    m_transformGizmoTable(db.createTable<TransformGizmo>())
{
}

EditorSystem::~EditorSystem()
{
}

void EditorSystem::update(const Scene&)
{
}

void EditorSystem::processInput(const FrameInput& input)
{
    //
    // Cycle transform gizmo operation
    //
    if (input.isKeyPressed(GLFW_KEY_SPACE))
    {
        static constexpr std::array<ImGuizmo::OPERATION, 4> operations =
        {
            ImGuizmo::OPERATION::TRANSLATE,
            ImGuizmo::OPERATION::ROTATE,
            ImGuizmo::OPERATION::SCALE
        };

        auto nextOperation = [&](const ImGuizmo::OPERATION& current)
        {
            bool notLast = static_cast<size_t>(current) < (operations.size() - 1);
            return operations[notLast ? current + 1 : 0];
        };

        m_transformGizmoTable.forEach([&](EntityId, TransformGizmo& gizmo) 
        {
            gizmo.operation = nextOperation(gizmo.operation);
        });
    }

    //
    // Toggle transform gizmo mode
    //
    if (input.isKeyPressed(GLFW_KEY_M))
    {
        m_transformGizmoTable.forEach([&](EntityId, TransformGizmo& gizmo)
        {
            gizmo.mode = gizmo.mode == ImGuizmo::MODE::LOCAL ?
                ImGuizmo::MODE::WORLD :
                ImGuizmo::MODE::LOCAL;
        });

        // With query:
        //query()
        //    .hasComponent(m_transformGizmoTable)
        //    .executeIds([&](EntityId, TransformGizmo& gizmo)
        //{
        //    gizmo.mode = gizmo.mode == ImGuizmo::MODE::LOCAL ?
        //        ImGuizmo::MODE::WORLD :
        //        ImGuizmo::MODE::LOCAL;
        //});
    }

    //
    // Delete selected
    //
    if (input.isKeyPressed(GLFW_KEY_DELETE))
    {
        query()
            .hasComponent<Selected>()
            .executeIds([&](EntityId id) 
        {
            markDeleted(id);
        });
    }

    //
    // Clear selected
    //
    if (input.isKeyPressed(GLFW_KEY_ESCAPE))
    {
        m_selectedTable.clear();
    }

    // Objects can only be selected if the cursor is 
    // neither captured nor over the transform gizmo 
    bool canSelectObjects = !input.cursorCaptured && !ImGuizmo::IsOver();

    //
    // Hover objects
    //
    if (canSelectObjects && input.cursorMoved)
    {
        // Always clear Hovered on mouse move
        m_hoveredTable.clear();

        auto camera = query().find<Camera>();
        assert(camera != nullptr && "No camera in scene");

        // Cast ray from cursor screen position to all meshes
        Ray ray = camera->screenPointToRay(input.cursorPositionNormalized);

        auto closestId = InvalidId;
        auto closestDistance = std::numeric_limits<float>::max();

        query()
            .hasComponent<Mesh>()
            .execute([&](
                EntityId id,
                const Mesh& mesh)
        {
            float d = gfx::raycast(ray, mesh.obb);
            if (d > 0.f && d < closestDistance)
            {
                closestId = id;
                closestDistance = d;
            }
        });

        // Assign Hovered to closest hit
        if (closestId != InvalidId)
        {
            m_hoveredTable.assign(closestId, Hovered());
        }

        //query()
        //    .hasComponent(m_hoveredTable)
        //    .executeIds([&](EntityId, Hovered& hovered)
        //{
        //    m_hoveredTable.clear();
        //    ...
        //});
    }

    //
    // Select objects
    //
    if (canSelectObjects && input.isButtonPressed(GLFW_MOUSE_BUTTON_1))
    {
        bool toggleSelection = input.isKeyDown(GLFW_KEY_LEFT_CONTROL) ||
                               input.isKeyDown(GLFW_KEY_RIGHT_CONTROL);
        if (toggleSelection)
        {
            m_hoveredTable.forEach([&](EntityId id, Hovered&)
            {
                if (m_selectedTable.check(id))
                {
                    m_selectedTable.remove(id);
                }
                else
                {
                    m_selectedTable.assign(id, Selected());
                }
            });
        }
        else
        {
            m_selectedTable.clear();
            m_hoveredTable.forEach([&](EntityId id, Hovered&)
            {
                m_selectedTable.assign(id, Selected());
            });
        }
    }
}
