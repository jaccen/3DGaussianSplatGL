#pragma once

#include <core/Datatypes.hpp>
#include <render/components/AABB.hpp>
#include <render/components/OBB.hpp>
#include <scene/components/Camera.hpp>

namespace jaccen
{
    namespace gfx
    {
        // Raycast algorithm based on an Oriented Bounding Box (OOB).
        //   'ray' : Ray origin and direction, in world space.
        //   'aabb' : Axis-aligned bounding box of the mesh to check against ray intersection.
        //   'modelMatrix' : Transformation applied to the mesh, which will thus be also applied to its bounding box.
        //   'intersectionDistance' : Additional output; distance between 'origin' and the intersection with the OBB
        // Returns true if the ray intersects with the oriented bounding box, otherwise false.
        bool raycastObb(
            const Ray& ray,
            const AABB& aabb,
            const mat4& modelMatrix,
            float& intersectionDistance);

        // Algorithms taken from "Game Physics Cookbook" chapter "3D Line Intersections":

        // Raycast algorithm for checking intersection against an axis-aligned bounding box.
        //   'ray' : Ray origin and direction, in world space.
        //   'aabb' : Axis-aligned bounding box to check against ray intersection.
        // Returns the closest distance between the ray origin and the intersection with
        // the bounding box, or -1.0 if there is no intersection.
        float raycast(const Ray& ray, const AABB& aabb);

        // Raycast algorithm for checking intersection against an oriented bounding box.
        //   'ray' : Ray origin and direction, in world space.
        //   'obb' : Oriented bounding box to check against ray intersection.
        // Returns the closest distance between the ray origin and the intersection with
        // the bounding box, or -1.0 if there is no intersection.
        float raycast(const Ray& ray, const OBB& obb);
    }
}
