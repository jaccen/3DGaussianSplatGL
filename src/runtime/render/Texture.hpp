#pragma once

#include <core/Core.hpp>
#include <render/Id.hpp>

namespace jaccen
{
    namespace gfx
    {
        enum class FilterType
        {
            Nearest = 0,
            Linear,
            NearestMipmapNearest,
            LinearMipmapNearest,
            NearestMipmapLinear,
            LinearMipmapLinear
        };

        enum class WrapType
        {
            Repeat = 0,
            MirroredRepeat,
            ClampToEdge,
            MirrorClampToEdge
        };
        struct Params
        {
            FilterType minFilter;
            FilterType magFilter;
            WrapType sWrap;
            WrapType tWrap;
        };

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

            void  Bind(int unit) const;
            const Id& id() const { return m_id; }

        private:
            // The OpenGL texture ID.
            Id m_id = Id::Invalid;
            bool hasAlphaChannel;
        };
    }
}