#pragma once

#include <core/Core.hpp>
#include <render/Id.hpp>

namespace jaccen
{
    namespace gfx
    {
        class Texture
        {
        public:
            Texture(const std::string& path, int wrap, int filter, unsigned format);
            ~Texture();
            // Move only.
            Texture(const Texture&) = delete;
            Texture& operator=(const Texture&) = delete;
            Texture(Texture&& other) = default;
            Texture& operator=(Texture&& other) = default;

            const Id& id() const { return m_id; }

        private:
            // The OpenGL texture ID.
            Id m_id = Id::Invalid;
        };
    }
}