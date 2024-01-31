#include <render/Texture.hpp>

#include <core/Core.hpp>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace jaccen;
using namespace jaccen::gfx;

Texture::Texture(const std::string& path, int wrap, int filter, unsigned format)
{
    glGenTextures(1, &m_id.uint());
    glBindTexture(GL_TEXTURE_2D, m_id);

    // Set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLint>(filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLint>(filter));

    stbi_set_flip_vertically_on_load(true);

    // Load and generate the texture
    int width;
    int height;
    int numberOfChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &numberOfChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, static_cast<GLenum>(format), GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR::TEXTURE::Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

Texture::~Texture()
{
    if (m_id.valid())
    {
        glDeleteTextures(1, &m_id.uint());
    }
}
void Texture::Bind(int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_id);
}
