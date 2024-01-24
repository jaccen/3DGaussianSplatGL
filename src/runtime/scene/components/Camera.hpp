#pragma once

#include <core/Core.hpp>

namespace jaccen
{
    struct Ray
    {
        // Ray origin in world space.
        vec3 origin = vec3(0);
        // Ray direction in world space, normalized.
        vec3 direction = vec3(0);
    };

    struct Camera
    {
        static const vec3 WorldUp;

        float fov = 45.0f;
        float aspectRatio = 1.0f;
        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        // todo: these could be cached in a separate CameraMatrix component
        mat4 viewMatrix = mat4(1.0f);
        mat4 projectionMatrix = mat4(1.0f);

        // Transform screen position in Normalized Device Coordinates (NDC) into a ray with
        // a world space origin and direction.
        //   'normalizedPosition' : Screen position to transform in range [-1,-1] to [1,1].
        // Returns a ray representing the screen position.
        Ray screenPointToRay(double2 normalizedPosition) const;
    };
}

