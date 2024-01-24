#include <scene/CameraSystem.hpp>

#include <scene/components/Transform.hpp>

using namespace jaccen;

CameraSystem::CameraSystem(Database& db) :
    m_cameraTable(db.createTable<Camera>()),
    m_cameraControlTable(db.createTable<CameraControl>())
{
}

CameraSystem::~CameraSystem()
{
}

void CameraSystem::registerWindow(Window& window)
{
    m_cameraController = std::make_shared<CameraController>(window.size());

    window.addEventListener(m_cameraController);
    window.onWindowResize([&](int2 size)
    {
        // TODO: Create Window/WindowState component? Could use Updated, Added...
        query()
            .hasComponent<Camera>(m_cameraTable)
            .execute([&](
                EntityId,
                Camera& camera)
                {
                    camera.aspectRatio = static_cast<float>(size.x) / size.y;
                });
    });
}

void CameraSystem::update(const Scene&)
{
    assert(m_cameraController && "Window not registered");

    m_cameraControlTable.forEach(
        [&](EntityId id, CameraControl& control) 
    {
        if (m_cameraController->update(control))
        {
            markUpdated(id);
        }
    });

    query()
        .hasComponent<Updated>()
        .hasComponent<CameraControl>()
        .hasComponent<Camera>(m_cameraTable)
        .execute([&](
            EntityId, 
            const Updated&,
            const CameraControl& control,
            Camera& camera)
    {
        camera.fov -= control.zoom;
        camera.fov = math::clamp(camera.fov, 10.0f, 45.0f);
    });

    query()
        .hasComponent<Updated>()
        .hasComponent<Transform>()
        .hasComponent<Camera>(m_cameraTable)
        .execute([&](
            EntityId,
            const Updated&,
            const Transform& transform,
            Camera& camera)
    {
        mat4 model = transform.modelMatrix();
        
        camera.viewMatrix = glm::inverse(model);
        camera.projectionMatrix = glm::perspective(
            glm::radians(camera.fov),
            camera.aspectRatio,
            camera.nearPlane,
            camera.farPlane);
    });
}
