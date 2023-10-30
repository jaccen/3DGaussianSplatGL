/**
 * MyApplication.hpp 
 * Contributors:
 *      * jaccen
 * Licence:
 *      * MIT
 */

#ifndef OPENGL_CMAKE_SKELETON_MYAPPLICATION
#define OPENGL_CMAKE_SKELETON_MYAPPLICATION
#include "vera/app.h"
#include "vera/ops/string.h"
#include "vera/ops/meshes.h"

using namespace std;
using namespace vera;
using namespace glm;

struct PointCloud {
  std::vector<glm::vec3> _points;
  std::vector<glm::vec3> _normals;
  std::vector<glm::vec3> _colors;
};
class MyApplication : public App{
 public:
   MyApplication();
  // bool loadPly(const std::string& filename);
 protected:
   virtual void setup();
   virtual void draw();
   virtual void update();
 private:
  const int size = 100;

  // shader
  Shader vertexShader;
  Shader fragmentShader;
  // shader matrix uniform
  glm::mat4 projection = glm::mat4(1.0);
  glm::mat4 view = glm::mat4(1.0);

  // VBO/VAO/ibo
  GLuint vao, vbo, ibo;

  PointCloud _point_cloud;

 private:
  Vbo world;
  Shader world_shader;
  Texture world_texture;

  Vbo satellite;
  vec3 satellite_pos;
  vector<vec3> orbit;

  Light sun;
  Camera cam;
};

#endif  // OPENGL_CMAKE_SKELETON_MYAPPLICATION
