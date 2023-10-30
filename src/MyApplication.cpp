/**
 * MyApplication.cpp
 * Contributors:
 *      * jaccen  Email:jaccen2007@163.com
 * Licence:
 *      * MIT
 */
#include "MyApplication.hpp"
#include <iostream>
#include <vector>
#include <fstream>

struct VertexType {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
};

float heightMap(const glm::vec2 position) {
  return 2.0 * sin(position.x) * sin(position.y);
}

VertexType getHeightMap(const glm::vec2 position) {
  const glm::vec2 dx(1.0, 0.0);
  const glm::vec2 dy(0.0, 1.0);

  VertexType v;
  float h = heightMap(position);
  float hx = 100.f * (heightMap(position + 0.01f * dx) - h);
  float hy = 100.f * (heightMap(position + 0.01f * dy) - h);

  v.position = glm::vec3(position, h);
  v.normal = glm::normalize(glm::vec3(-hx, -hy, 1.0));

  float c = sin(h * 5.f) * 0.5 + 0.5;
  v.color = glm::vec4(c, 1.0 - c, 1.0, 1.0);
  return v;
}

MyApplication::MyApplication(){
}


void MyApplication::setup() {
  string frag = R"(
            #ifdef GL_ES
            precision mediump float;
            #endif

            uniform sampler2D   u_tex0;
            uniform vec3        u_light;

            #ifdef MODEL_VERTEX_NORMAL
            varying vec3        v_normal;
            #endif

            #ifdef MODEL_VERTEX_TEXCOORD
            varying vec2        v_texcoord;
            #endif

            void main () {
                vec3 color = vec3(1.);

                #ifdef MODEL_VERTEX_TEXCOORD
                color = vec3( 0.5 + texture2D(u_tex0, v_texcoord).r * 0.5);
                #endif
                
                #ifdef MODEL_VERTEX_NORMAL
                float shade = dot(v_normal, normalize(u_light));
                shade = smoothstep(-0.25, 0.25, shade);
                color *= 0.2 + shade * 0.8;
                #endif

                gl_FragColor = vec4(color, 1.);
            }
        )";

  world_shader = createShader(frag);
  world.load(sphereMesh());

  satellite.load(boxMesh(0.075f, 0.075f, 0.075f));

  world_texture.load("earth-water.png");

  setCamera(cam);
  cam.setPosition(vec3(0.0f, 0.0f, -4.0f));
  cam.lookAt(vec3(0.0f, 0.1f, 0.0f));

  sun.setPosition(vec3(1.0f, 1.0f, 1.0f));
  sun.setType(LIGHT_POINT);
  addLight(sun);
  lights();

  textAlign(ALIGN_CENTER);
  textAlign(ALIGN_BOTTOM);
  textSize(28.0f);

  background(0.0);
  blendMode(BLEND_ALPHA);
}

void MyApplication::draw() {
  orbitControl();

  sun.setPosition(vec3(cos(frameCount * 0.01f), 0.0, sin(frameCount * 0.01f)));

  push();
  rotateY(frameCount * 0.0025f);
  shader(world_shader);
  texture(world_texture);
  model(world);
  pop();

  push();
  rotateY(frameCount * 0.0035f);
  rotateX(frameCount * 0.005f);
  translate(0.0f, 0.0f, 1.2f);
  fill(0.75f + sin(millis() * 0.005f) * 0.25f, 0.0f, 0.0f);
  model(satellite);
  satellite_pos = vec3(getWorldMatrix() * vec4(0.0f, 0.0f, 1.2f, 0.0f));
  pop();

  strokeWeight(1.0);
  stroke(0.75f, 0.0f, 0.0f);
  line(orbit);

  fill(1.0f);
  text("Hello World", width * 0.5f, height * 0.95f);
}

void MyApplication::update() {
  if (frameCount % 15 == 0) {
    orbit.push_back(satellite_pos);
    if (orbit.size() > 500)
      orbit.erase(orbit.begin());
  }
}

