#pragma once

#include <Eigen/Core>
#include <boost/filesystem.hpp>

namespace pxl {

class Material {
 public:
  Material();
  virtual ~Material() {}

 public:
  Eigen::Vector3f ambient;
  Eigen::Vector3f diffuse;
  Eigen::Vector3f specular;
  float shininess;
  boost::filesystem::path diffuse_texture_path;
};
}  // namespace pxl