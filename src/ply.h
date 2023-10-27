#pragma once

#include "logging.h"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
//#include <tinyply.h>
#include <unordered_map>
#include <vector>
#include "../external/tinyply/source/tinyply.h"
struct Point {
  float x;
  float y;
  float z;
};

struct Normal {
  float x;
  float y;
  float z;
};

struct Color {
  unsigned char r;
  unsigned char g;
  unsigned char b;
};
template <typename T>
T read_binary_value(std::istream& file) {
  T value;
  file.read(reinterpret_cast<char*>(&value), sizeof(T));
  return value;
}

// TODO: Do something with the images vector
// adapted from
// https://github.com/colmap/colmap/blob/dev/src/colmap/base/reconstruction.cc
struct ImagePoint {  // we dont need this later
  double _x;
  double _y;
  uint64_t _point_id;
};
struct PointCloud {
  std::vector<Point> _points;
  std::vector<Normal> _normals;
  std::vector<Color> _colors;
};
class PlyLoader {
 public:
  PlyLoader();
  ~PlyLoader();

  PointCloud read_ply_file(std::filesystem::path file_path);
  std::vector<Image> read_images_binary(std::filesystem::path file_path); 
 private:
};

