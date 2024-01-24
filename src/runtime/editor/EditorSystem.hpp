#pragma once

#include <core/ecs/System.hpp>
#include <editor/Components.hpp>

namespace jaccen
{
    class FrameInput;

    class EditorSystem : public System
    {
    public:
        ADD_COMPONENT_FUNCTION(Hovered, m_hoveredTable);
        ADD_COMPONENT_FUNCTION(Selected, m_selectedTable);
        ADD_COMPONENT_FUNCTION(TransformGizmo, m_transformGizmoTable);

    public:
        EditorSystem(Database& db);
        ~EditorSystem();

        void update(const Scene& scene) override;
        
        void processInput(const FrameInput& input);

        // QUERY:  'toggleGizmo'
        // READS:  <input>
        // WRITES: TransformGizmo

        // QUERY:  'toggleGizmoMode'
        // READS:  <input>
        // WRITES: TransformGizmo

        // QUERY:  'deleteSelected'
        // READS:  Selected
        // WRITES: (Deleted)

        // QUERY:  'clearSelected'
        // READS:  <input>
        // WRITES: Selected

        // QUERY:  'hoverOn(double2 pos)'
        // READS:  Camera, Mesh
        // WRITES: Hovered
        
        // QUERY:  'selectHovered'
        // READS:  Hovered
        // WRITES: Selected

    private:
        TableRef<Hovered> m_hoveredTable;
        TableRef<Selected> m_selectedTable;
        TableRef<TransformGizmo> m_transformGizmoTable;
    };
}