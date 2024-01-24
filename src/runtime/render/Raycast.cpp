#include <render/Raycast.hpp>

#include <core/Core.hpp>

// http://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-custom-ray-obb-function/
// Additional reference: http://antongerdelan.net/opengl/raycasting.html
bool jaccen::gfx::raycastObb(
    const Ray& ray,
    const AABB& aabb,
    const mat4& modelMatrix, 
    float& intersectionDistance)
{
    // Largest or farthest "near" intersection found
    float minDistance = 0.0f;
    // Smallest or nearest "far" intersection found
    float maxDistance = 100000.0f;

    vec3 obbPositionWorldspace(
        modelMatrix[3].x, 
        modelMatrix[3].y, 
        modelMatrix[3].z);

    // Delta used to compute the intersections with the planes
    vec3 delta = obbPositionWorldspace - ray.origin;

    // Make a function to test intersection with the two planes 
    // perpendicular to a specific OBB axis
    auto testIntersection = [
        &ray,
        &delta,
        &minDistance, 
        &maxDistance](
            vec3 axis, 
            float aabbMin,
            float aabbMax)
    {
        float e = glm::dot(axis, delta);
        float f = glm::dot(ray.direction, axis);
        constexpr float epsilon = 0.001f;

        if (std::abs(f) > epsilon)
        {
            // Intersection with the "left" plane
            float tMin = (e + aabbMin) / f; 
            // Intersection with the "right" plane
            float tMax = (e + aabbMax) / f; 

            // tMin and tMax now contain distances between ray origin and ray-plane intersections, 
            // but we don�t know in what order, so we make sure that tMin represents the near
            // intersection and tMax the far
            if (tMin > tMax)
            {
                float temp = tMin; 
                tMin = tMax; 
                tMax = temp;
            }

            if (tMax < maxDistance)
            {
                maxDistance = tMax;
            }

            if (tMin > minDistance)
            {
                minDistance = tMin;
            }

            // If "far" is closer than "near", then there is NO intersection
            if (minDistance > maxDistance)
            {
                return false;
            }
        }
        else 
        {
            // Rare case: the ray is almost parallel to the planes,
            // so they don't have any "intersection"
            if (-e + aabbMin > 0.0f || -e + aabbMax < 0.0f)
            {
                return false;
            }
        }

        // Intersection is possible
        return true;
    };

    if (!testIntersection(
        vec3(
            modelMatrix[0].x,
            modelMatrix[0].y,
            modelMatrix[0].z),
        aabb.min().x,
        aabb.max().x))
    {
        return false;
    }
        
    if (!testIntersection(
        vec3(
            modelMatrix[1].x,
            modelMatrix[1].y,
            modelMatrix[1].z),
        aabb.min().y,
        aabb.max().y))
    {
        return false;
    }
        
    if (!testIntersection(
        vec3(
            modelMatrix[2].x,
            modelMatrix[2].y,
            modelMatrix[2].z),
        aabb.min().z,
        aabb.max().z))
    {
        return false;
    }

    intersectionDistance = minDistance;
    return true;
}

float jaccen::gfx::raycast(const Ray&, const AABB&)
{
    // TODO
    return 0.0f;
}

float jaccen::gfx::raycast(const Ray& ray, const OBB& obb)
{
    // Store half extents of the box
    vec3 size = obb.halfExtents;

    // Store each axis (X, Y, Z) of the OBB's rotation frame
    vec3 x = obb.rotation[0];
    vec3 y = obb.rotation[1];
    vec3 z = obb.rotation[2];

    // To test slabs, we first need to find a vector pointing from the origin
    // of the ray to the OBB
    vec3 p = obb.position - ray.origin;

    // Next, we project the direction of the ray onto each of the axis of the OBB
    vec3 f(
        glm::dot(x, ray.direction),
        glm::dot(y, ray.direction),
        glm::dot(z, ray.direction));

    // We project 'p' into every axis of the OBB's rotation frame,
    // and store the result of each of these projections
    vec3 e(
        glm::dot(x, p),
        glm::dot(y, p),
        glm::dot(z, p));

    // Next, we calculate the min and max intersection distances for each axis
    // (tx_min, tx_max, ty_min, ty_max, tz_min, tz_max), and store these values
    // in t[0..5]
    float t[6] = { 0.f, 0.f, 0.f, 0.f, 0.f, 0.f };

    constexpr float epsilon = 0.00001f;

    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(f[i]) < epsilon)
        {
            // If the ray is parallel to the slab being tested, and the origin
            // of the ray is not inside the slab, we have no hit
            if (-e[i] - size[i] > 0.f || -e[i] + size[i] < 0.f)
            {
                return -1.f;
            }
            f[i] = epsilon; // Avoid div by 0!
        }

        t[i * 2 + 0] = (e[i] + size[i]) / f[i]; // min
        t[i * 2 + 1] = (e[i] - size[i]) / f[i]; // max
    }

    // If the above loop finished executing, the ray hit all three slabs.
    // To finish the raycast, we find the largest minimum (tmin) and smallest 
    // maximum (tmax). We take care of any edge cases and return the point
    // closes to the origin of the ray.
    float tmin = std::fmaxf(
        std::fmaxf(
            std::fminf(t[0], t[1]),  // x
            std::fminf(t[2], t[3])), // y
        std::fminf(t[4], t[5]));     // z

    float tmax = std::fminf(
        std::fminf(
            std::fmaxf(t[0], t[1]),  // x
            std::fmaxf(t[2], t[3])), // y
        std::fmaxf(t[4], t[5]));     // z

    // If 'tmax' is less than 0, the ray is intersecting the OBB in the negative
    // direction. This means the OBB is behind the origin of the ray, and this
    // should not count as an intersection.
    if (tmax < 0.f)
    {
        return -1.f;
    }

    // If 'tmin' is greater than 'tmax', the ray does not intersect the OBB
    if (tmin > tmax)
    {
        return -1.f;
    }

    // If 'tmin' is less than 0, the ray started inside the OBB. This means
    // 'tmax' is a valid intersection.
    if (tmin < 0.f)
    {
        return tmax;
    }

    return tmin;
}
