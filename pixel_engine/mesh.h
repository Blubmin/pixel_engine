#pragma once

#include <vector>

#include <Eigen/Core>

namespace pxl {
class Mesh {
 public:
  Mesh();

  std::vector<Eigen::Vector3i> triangles;
  std::vector<Eigen::Vector3f> positions;
  std::vector<Eigen::Vector3f> normals;
  std::vector<Eigen::Vector2f> texture_coordinates;
};
}  // namespace pxl
