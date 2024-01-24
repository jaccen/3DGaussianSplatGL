#pragma once

#include <core/Core.hpp>

namespace jaccen
{
    struct Transform
    {
        vec3 position = vec3(0.0f, 0.0f, 0.0f);
        qua	 rotation = qua(1.0f, 0.0f, 0.0f, 0.0f);
        vec3 scale    = vec3(1.0f, 1.0f, 1.0f);

        Transform() = default;
        Transform(vec3 pos) : position(pos) {}
        Transform(vec3 pos, qua rot) : position(pos), rotation(rot) {}
        Transform(vec3 pos, qua rot, vec3 scale) : position(pos), rotation(rot), scale(scale) {}

        mat4 modelMatrix() const
        {
            mat4 translate = glm::translate(mat4(1.0f), position);
            mat4 rotate = glm::mat4_cast(rotation);
            mat4 scale_ = glm::scale(mat4(1.0f), scale);

            return translate * rotate * scale_;
        }

        static Transform identity() 
        {
            return Transform(
                vec3(0.0f, 0.0f, 0.0f),
                qua(1.0f, 0.0f, 0.0f, 0.0f), 
                vec3(0.0f, 0.0f, 0.0f));
        }
    };

    inline bool operator==(const Transform& lhs, const Transform& rhs)
    {
        return lhs.position == rhs.position &&
               lhs.rotation == rhs.rotation &&
               lhs.scale == rhs.scale;
    }

    inline bool operator!=(const Transform& lhs, const Transform& rhs)
    {
        return !(lhs == rhs);
    }
}