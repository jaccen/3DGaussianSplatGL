#pragma once

#include <core/ecs/System.hpp>

#include <editor/Components.hpp>

#include <scene/components/Camera.hpp>
#include <scene/components/CameraControl.hpp>
#include <scene/components/Transform.hpp>

namespace jaccen
{
    class AABB;

    class TransformSystem : public System
    {
    public:
        ADD_COMPONENT_FUNCTION(Transform, m_transformTable);

    public:
        TransformSystem(Database& db);
        ~TransformSystem();

        void update(const Scene& scene) override;

        
        void schedule(Scheduler& scheduler);

        void transformCamera(
            const CameraControl& cameraControl,
            Transform& transform);

        Transform transformGizmo(
            const Camera& camera,
            const AABB& selectedBounds,
            const TransformGizmo& transformGizmo, 
            Transform& transform);

        // QUERY:  'translateCamera'
        // READS:  (Updated), CameraControl, Transform
        // WRITES: Transform

        // QUERY:  'computeSelectedBounds'
        // READS:  Selected, Transform
        // WRITES: <bounds>

        // QUERY:  'translateSelected'
        // READS:  <bounds>, Camera, TransformGizmo, Transform, Selected
        // WRITES: Transform, (Updated)

    private:
        TableRef<Transform> m_transformTable;
    };
}