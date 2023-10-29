#ifndef TEXTURE_H
#define TEXTURE_H
#include <gl/gl.h>
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "stb_image.h"
using namespace std;
class Texture {
 public:
  Texture() {}

  Texture(unsigned int ourTexture[], int ourLimit);

  void GeneraTextura(int n, string path, int e);

  void ViewTexture();

  void ViewTextureMap(int num, unsigned int nameMap);

  string UniformTexture();

  unsigned int loadTextureID(string path, int e);

 public:
  unsigned int textureID = 0;
  unsigned int width = 0;
  unsigned int hright = 0;
  int limite;
  unsigned int texture[1];
};
#endif