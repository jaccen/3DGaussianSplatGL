#include <scene/TransformSystem.hpp>

#include <core/Core.hpp>
#include <scene/Scene.hpp>
#include <ui/ImGui.hpp>

#include <ImGuizmo.h>

using namespace jaccen;

TransformSystem::TransformSystem(
    Database& db) :
    m_transformTable(db.createTable<Transform>())
{
}

TransformSystem::~TransformSystem()
{
}

void TransformSystem::schedule(Scheduler&)
{
    // TODO: Scheduler
    //query()
    //    .hasComponent<Updated>()
    //    .hasComponent<CameraControl>()
    //    .hasComponent<Transform>()
    //    .executeIds([&](EntityId)
    //{
    //    scheduler
    //        .job()
    //        .read<CameraControl>()
    //        .readWrite<Transform>(m_transformTable)
    //        .onExecute(transformCamera);
    //});

    //auto& translateCamera = scheduler
    //    .job()
    //    .read<Updated>()
    //    .read<CameraControl>()
    //    .readWrite<Transform>(m_transformTable);

    //AABB selectedBounds;

    //auto& computeBounds = scheduler
    //    .job()
    //    .require(translateCamera)
    //    .read<Selected>()
    //    .read<Transform>()
    //    .readWrite<AABB>(selectedBounds);
}

void TransformSystem::update(const Scene&)
{
    // Move and rotate camera
    // TODO: Consider extending CameraControl into a "TransformControl" component 
    // which allows input and program control to any entity with a transform.
    // With this it might be wise to expect that the front vector is precomputed.
    query()
        .hasComponent<Updated>()
        .hasComponent<CameraControl>()
        .hasComponent<Transform>(m_transformTable)
        .execute([&](
            EntityId, 
            const Updated&,
            const CameraControl& control,
            Transform& transform)
    {
        transformCamera(control, transform);
    });

    // Compute bounds for selected objects
    AABB selectedBounds; // TODO: Don't calculate every frame

    query()
        .hasComponent<Selected>()
        .hasComponent<Transform>()
        .execute([&](
            EntityId,
            const Selected&,
            const Transform& transform)
    {
        selectedBounds.expand(transform.position);
    });

    // Early out if we have no selection
    if (!selectedBounds.valid())
    {
        return;
    }

    auto camera = query().find<Camera>();
    assert(camera != nullptr && "No camera in scene");

    Transform transformGizmoDelta;
    
    // Move, rotate, and scale transform gizmo
    query()
        .hasComponent<TransformGizmo>()
        .hasComponent<Transform>(m_transformTable)
        .execute([&](
            EntityId,
            const TransformGizmo& gizmo,
            Transform& transform)
    {
        // Move gizmo to selection center
        transform.position = selectedBounds.center();

        transformGizmoDelta = transformGizmo(
            *camera, 
            selectedBounds,
            gizmo,
            transform);
    });

    if (transformGizmoDelta == Transform::identity())
    {
        return;
    }

    // Apply delta transform to selected objects
    query()
        .hasComponent<Selected>()
        .hasComponent<Transform>(m_transformTable)
        .execute([&](
            EntityId id,
            const Selected&,
            Transform& transform)
    {
        // Note the order of addition for rotation
        transform.position += transformGizmoDelta.position;
        transform.rotation = transformGizmoDelta.rotation * transform.rotation;
        transform.scale += transformGizmoDelta.scale;

        markUpdated(id);
    });
}

void TransformSystem::transformCamera(
    const CameraControl& cameraControl, 
    Transform& transform)
{
    vec3 cameraFront;
    cameraFront.x = cos(glm::radians(cameraControl.pitch)) * cos(glm::radians(cameraControl.yaw));
    cameraFront.y = sin(glm::radians(cameraControl.pitch));
    cameraFront.z = cos(glm::radians(cameraControl.pitch)) * sin(glm::radians(cameraControl.yaw));
    glm::normalize(cameraFront);

    vec3 cameraRight = glm::normalize(glm::cross(cameraFront, Camera::WorldUp));
    vec3 cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    float cameraSpeed = cameraControl.speed * Time::deltaTime();

    if (cameraControl.isMoving(CameraMovement::Forward))
    {
        transform.position += cameraFront * cameraSpeed;
    }

    if (cameraControl.isMoving(CameraMovement::Backward))
    {
        transform.position -= cameraFront * cameraSpeed;
    }

    if (cameraControl.isMoving(CameraMovement::Left))
    {
        transform.position -= glm::normalize(
            glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    if (cameraControl.isMoving(CameraMovement::Right))
    {
        transform.position += glm::normalize(
            glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }

    transform.rotation = glm::quatLookAt(cameraFront, Camera::WorldUp);
}

Transform TransformSystem::transformGizmo(
    const Camera& camera,
    const AABB& /*selectedBounds*/,
    const TransformGizmo& transformGizmo,
    Transform& transform)
{
    Transform previousTransform(transform);
    mat4 modelMatrix(transform.modelMatrix());

    // Manipulate the gizmo's model matrix with ImGuizmo
    bool gizmoUsed = imgui::gizmoManipulate(
        modelMatrix,
        camera.viewMatrix,
        camera.projectionMatrix,
        transformGizmo.operation,
        transformGizmo.mode);

    if (!gizmoUsed)
    {
        return Transform::identity();
    }

    // Decompose manipulated values back into component
    vec3 skew;
    vec4 perspective;
    glm::decompose(
        modelMatrix,
        transform.scale,
        transform.rotation,
        transform.position,
        skew,
        perspective);

    // Compute delta transform
    Transform delta;
    delta.position = transform.position - previousTransform.position;
    delta.rotation = transform.rotation * glm::inverse(previousTransform.rotation);
    delta.scale = transform.scale - previousTransform.scale;

    return delta;
}