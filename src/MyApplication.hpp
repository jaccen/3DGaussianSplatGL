/**
 * MyApplication.hpp 
 * Contributors:
 *      * jaccen
 * Licence:
 *      * MIT
 */

#ifndef OPENGL_CMAKE_SKELETON_MYAPPLICATION
#define OPENGL_CMAKE_SKELETON_MYAPPLICATION

#include "Application.hpp"
#include "Shader.hpp"
struct PointCloud {
  std::vector<glm::vec3> _points;
  std::vector<glm::vec3> _normals;
  std::vector<glm::vec3> _colors;
};
class MyApplication : public Application {
 public:
  MyApplication();
  bool loadPly(const std::string& filename);
 protected:
  virtual void loop();

 private:
  const int size = 100;

  // shader
  Shader vertexShader;
  Shader fragmentShader;
  ShaderProgram shaderProgram;

  // shader matrix uniform
  glm::mat4 projection = glm::mat4(1.0);
  glm::mat4 view = glm::mat4(1.0);

  // VBO/VAO/ibo
  GLuint vao, vbo, ibo;

  PointCloud _point_cloud;
};

#endif  // OPENGL_CMAKE_SKELETON_MYAPPLICATION
