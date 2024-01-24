#include <render/components/OBB.hpp>

#include <core/Core.hpp>

using namespace jaccen;

OBB::OBB() :
    position(0.0f),
    halfExtents(1.0f),
    rotation(1.0f)
{
}

OBB::OBB(const vec3& position, const vec3& size) :
    position(position),
    halfExtents(size),
    rotation(1.0f)
{
}

OBB::OBB(const vec3& position, const vec3& size, const mat3& rotation) :
    position(position),
    halfExtents(size),
    rotation(rotation)
{
}

//OBB::OBB(const AABB &aabb, const mat4& mat)
//{
//    vec3 center(
//        mat[3].x,
//        mat[3].y,
//        mat[3].z);
//
//    vec4 min = vec4(aabb.min(), 1.0f) * mat;
//    min /= min.w;
//
//    vec4 max = vec4(aabb.max(), 1.0f) * mat;
//    max /= max.w;
//
//    m_min = min;
//    m_max = max;
//}

//void OBB::expand(const vec3& point)
//{
//    if (m_min.x > point.x) m_min.x = point.x;
//    if (m_min.y > point.y) m_min.y = point.y;
//    if (m_min.z > point.z) m_min.z = point.z;
//
//    if (m_max.x < point.x) m_max.x = point.x;
//    if (m_max.y < point.y) m_max.y = point.y;
//    if (m_max.z < point.z) m_max.z = point.z;
//}
//
//void OBB::clear()
//{
//    m_min = vec3(std::numeric_limits<float>::max());
//    m_max = vec3(std::numeric_limits<float>::min());
//}

//bool OBB::valid() const
//{
//    return m_min.x <= m_max.x &&
//           m_min.y <= m_max.y &&
//           m_min.z <= m_max.z;
//}
//
//vec3 OBB::center() const
//{
//    return vec3(
//        m_min.x + ((m_max.x - m_min.x) / 2),
//        m_min.y + ((m_max.y - m_min.y) / 2),
//        m_min.z + ((m_max.z - m_min.z) / 2));
//}