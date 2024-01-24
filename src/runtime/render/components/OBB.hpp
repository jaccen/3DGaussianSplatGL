#pragma once

#include <core/Datatypes.hpp>

namespace jaccen
{
    // Oriented bounding box.
    struct OBB
    {
    public:
        vec3 position;
        vec3 halfExtents;
        mat3 rotation;

    public:
        OBB();
        OBB(const vec3& position, const vec3& halfExtents);
        OBB(const vec3& position, const vec3& halfExtents, const mat3& rotation);
    };
}
