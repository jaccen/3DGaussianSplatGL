#include <render/components/AABB.hpp>

#include <core/Core.hpp>

using namespace jaccen;

AABB::AABB()
{
    clear();
}

void AABB::expand(const vec3& point)
{
    if (m_min.x > point.x) m_min.x = point.x;
    if (m_min.y > point.y) m_min.y = point.y;
    if (m_min.z > point.z) m_min.z = point.z;

    if (m_max.x < point.x) m_max.x = point.x;
    if (m_max.y < point.y) m_max.y = point.y;
    if (m_max.z < point.z) m_max.z = point.z;
}

void AABB::clear()
{
    m_min = vec3(std::numeric_limits<float>::max());
    m_max = vec3(std::numeric_limits<float>::lowest());
}

bool AABB::valid() const
{
    return m_min.x <= m_max.x &&
           m_min.y <= m_max.y &&
           m_min.z <= m_max.z;
}

vec3 AABB::center() const
{
    return (m_max + m_min) * 0.5f;
}

vec3 AABB::halfExtents() const
{
    return (m_max - m_min) * 0.5f;
}