#pragma once

#include <core/ecs/Database.hpp>

#include <editor/EditorSystem.hpp>

#include <render/RenderSystem.hpp>

#include <scene/CameraSystem.hpp>
#include <scene/TransformSystem.hpp>

namespace jaccen
{
    class Scene : public trait::no_copy
    {
    public:
        Scene();
        virtual ~Scene();
        Scene(Scene&&) = default;
        Scene& operator=(Scene&&) = default;

        void registerSystem(ISystem& system);
        void update(const FrameInput& input);
        void render();

        // TODO: entity creation, move into factory class?
        EntityId createEntity();
        EntityId createCamera();
        EntityId createCube(vec3 position);
        EntityId createGizmo();

        RenderSystem& renderSystem() { return m_renderSystem; }
        CameraSystem& cameraSystem() { return m_cameraSystem; }
        EditorSystem& editorSystem() { return m_editorSystem; }

        // Read-only access to scene database. Only systems are
        // allowed to modify database content for their own tables.
        const Database& database() const { return m_database; }

    private:
        Database m_database;
        std::vector<ISystem*> m_systems;

        TransformSystem m_transformSystem;
        RenderSystem m_renderSystem;
        CameraSystem m_cameraSystem;
        EditorSystem m_editorSystem;
    };
}
