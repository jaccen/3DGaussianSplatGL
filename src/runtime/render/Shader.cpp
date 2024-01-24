#include <render/Shader.hpp>

#include <glad/glad.h>

using namespace jaccen;
using namespace jaccen::gfx;

std::string readFile(const std::string& path)
{
    std::ifstream fstream;
    // Ensure ifstream object can throw exceptions
    fstream.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    fstream.open(path.c_str());

    std::stringstream sstream;
    sstream << fstream.rdbuf();
    fstream.close();

    return sstream.str();
}

unsigned compileShader(const std::string& file, unsigned type)
{
    std::string source = readFile(file);
    const char* src = source.c_str();
    
    unsigned shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char buffer[512];
        glGetShaderInfoLog(shader, 512, NULL, buffer);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED  " << file << std::endl;
        std::cout << buffer << std::endl;
    }
    assert(success);
    return shader;
}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    unsigned vertexShader = compileShader(vertexPath, GL_VERTEX_SHADER);
    unsigned fragmentShader = compileShader(fragmentPath, GL_FRAGMENT_SHADER);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    int  success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char buffer[512];
        glGetProgramInfoLog(m_id, 512, NULL, buffer);
        std::cout << "ERROR::SHADER::LINKING_FAILED" << std::endl;
        std::cout << "\t" << vertexPath << std::endl;
        std::cout << "\t" << fragmentPath << std::endl;
        std::cout << buffer << std::endl;
    }
    assert(success);

    // Delete shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader()
{
    if (m_id.valid())
    {
        glDeleteProgram(m_id);
    }
}

void Shader::use()
{
    glUseProgram(m_id);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int) value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, vec3 value) const
{
    auto location = glGetUniformLocation(m_id, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::setMat4(const std::string& name, mat4 value) const
{
    auto location = glGetUniformLocation(m_id, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}


