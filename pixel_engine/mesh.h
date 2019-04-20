#pragma once

#include <memory>
#include <vector>

#include <Eigen/Core>

#include <pixel_engine/entity.h>

namespace pxl {
class SubMesh {
 public:
  std::vector<uint32_t> triangles;
  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<float> texture_coordinates;
};

class Mesh : public Entity {
 public:
  virtual ~Mesh() {}
  virtual void Bind() {}
  virtual void Draw() {}

  std::vector<std::shared_ptr<SubMesh>> sub_meshes;
};
}  // namespace pxl
