#include <scene/components/Camera.hpp>

using namespace jaccen;

// By convention, OpenGL is a right-handed system:
// +x = right
// +y = up
// +z = back
// -x = left
// -y = down
// -z = front

const vec3 Camera::WorldUp = vec3(0.0f, 1.0f, 0.0f);

// https://github.com/opengl-tutorials/ogl/blob/master/misc05_picking/misc05_picking_custom.cpp
Ray Camera::screenPointToRay(double2 normalizedPosition) const
{
    mat4 inverseProjectionMatrix = glm::inverse(projectionMatrix);
    mat4 inverseViewMatrix = glm::inverse(viewMatrix);

    // The near plane maps to Z = -1 in Normalized Device Coordinates
    vec4 rayStartNdc(
        normalizedPosition.x,
        normalizedPosition.y,
        -1.0,
        1.0f);

    vec4 rayEndNdc(
        normalizedPosition.x,
        normalizedPosition.y,
        0.0,
        1.0f);

    vec4 rayStartCamera = inverseProjectionMatrix * rayStartNdc;
    rayStartCamera /= rayStartCamera.w;

    vec4 rayStartWorld = inverseViewMatrix * rayStartCamera;
    rayStartWorld /= rayStartWorld.w;

    vec4 rayEndCamera = inverseProjectionMatrix * rayEndNdc;
    rayEndCamera /= rayEndCamera.w;

    vec4 rayEndWorld = inverseViewMatrix * rayEndCamera;
    rayEndWorld /= rayEndWorld.w;

    vec4 rayDirection = glm::normalize(rayEndWorld - rayStartWorld);

    Ray ray;
    ray.origin = vec3(rayStartWorld);
    ray.direction = vec3(rayDirection);
    return ray;
}
