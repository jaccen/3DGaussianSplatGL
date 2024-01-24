#pragma once

#include <core/Datatypes.hpp>

namespace jaccen
{
    // Axis-aligned bounding box.
    class AABB
    {
    public:
        AABB();

        void expand(const vec3& point);
        void clear();
        
        bool valid() const;

        const vec3& min() const { return m_min; }
        const vec3& max() const { return m_max; }

        vec3 center() const;
        vec3 halfExtents() const;

    private:
        vec3 m_min;
        vec3 m_max;
    };
}
