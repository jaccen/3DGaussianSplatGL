#include "ply.h"
#include "logging.h"
#include<filesystem>
using namespace std::filesystem;

std::unique_ptr<std::istream> read_binary(std::filesystem::path file_path) {
  std::ifstream file(file_path, std::ios::binary);
  std::unique_ptr<std::istream> file_stream;
  if (file.fail()) {
    throw std::runtime_error("Failed to open file: " + file_path.string());
  }
  // preload
  std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(file), {});
  file_stream = std::make_unique<std::stringstream>(
      std::string(buffer.begin(), buffer.end()));
  return file_stream;
}

PlyLoader::PlyLoader() {}

PointCloud PlyLoader::read_ply_file(std::filesystem::path file_path) {
  auto ply_stream_buffer = read_binary(file_path);
  tinyply::PlyFile file;
  std::shared_ptr<tinyply::PlyData> vertices, normals, colors;
  file.parse_header(*ply_stream_buffer);

  //    std::cout << "\t[ply_header] Type: " << (file.is_binary_file() ?
  //    "binary" : "ascii") << std::endl; for (const auto& c :
  //    file.get_comments())
  //        std::cout << "\t[ply_header] Comment: " << c << std::endl;
  //    for (const auto& c : file.get_info())
  //        std::cout << "\t[ply_header] Info: " << c << std::endl;
  //
  //    for (const auto& e : file.get_elements()) {
  //        std::cout << "\t[ply_header] element: " << e.name << " (" << e.size
  //        << ")" << std::endl; for (const auto& p : e.properties) {
  //            std::cout << "\t[ply_header] \tproperty: " << p.name << "
  //            (type=" << tinyply::PropertyTable[p.propertyType].str << ")"; if
  //            (p.isList)
  //                std::cout << " (list_type=" <<
  //                tinyply::PropertyTable[p.listType].str << ")";
  //            std::cout << std::endl;
  //        }
  //    }
  // The header information can be used to programmatically extract properties
  // on elements known to exist in the header prior to reading the data. For
  // brevity, properties like vertex position are hard-coded:
  try {
    vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});
  } catch (const std::exception& e) {
    std::cerr << "tinyply exception: " << e.what() << std::endl;
  }

  try {
    normals =
        file.request_properties_from_element("vertex", {"nx", "ny", "nz"});
  } catch (const std::exception& e) {
    std::cerr << "tinyply exception: " << e.what() << std::endl;
  }

  try {
    colors = file.request_properties_from_element("vertex",
                                                  {"red", "green", "blue"});
  } catch (const std::exception& e) {
    std::cerr << "tinyply exception: " << e.what() << std::endl;
  }

  file.read(*ply_stream_buffer);

  PointCloud point_cloud;
  if (vertices) {
    std::cout << "\tRead " << vertices->count << " total vertices "
              << std::endl;
    try {
      point_cloud._points.resize(vertices->count);
      std::memcpy(point_cloud._points.data(), vertices->buffer.get(),
                  vertices->buffer.size_bytes());
    } catch (const std::exception& e) {
      std::cerr << "tinyply exception: " << e.what() << std::endl;
    }
  } else {
    std::cerr << "Error: vertices not found" << std::endl;
    exit(0);
  }

  if (normals) {
    std::cout << "\tRead " << normals->count << " total vertex normals "
              << std::endl;
    try {
      point_cloud._normals.resize(normals->count);
      std::memcpy(point_cloud._normals.data(), normals->buffer.get(),
                  normals->buffer.size_bytes());
    } catch (const std::exception& e) {
      std::cerr << "tinyply exception: " << e.what() << std::endl;
    }
  }

  if (colors) {
    std::cout << "\tRead " << colors->count << " total vertex colors "
              << std::endl;
    try {
      point_cloud._colors.resize(colors->count);
      std::memcpy(point_cloud._colors.data(), colors->buffer.get(),
                  colors->buffer.size_bytes());
    } catch (const std::exception& e) {
      std::cerr << "tinyply exception: " << e.what() << std::endl;
    }
  } else {
    std::cerr << "Error: colors not found" << std::endl;
    exit(0);
  }

  return point_cloud;
}
