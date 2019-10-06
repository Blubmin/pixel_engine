#pragma once

#include <memory>
#include <vector>

#include <Eigen/Core>

#include <pixel_engine/entity.h>
#include <pixel_engine/material.h>
#include <pixel_engine/program.h>

namespace pxl {
class SubMesh {
 public:
  std::vector<uint32_t> triangles;
  std::vector<float> positions;
  std::vector<float> normals;
  std::vector<float> texture_coordinates;
  uint32_t material_idx;
};

class Mesh : public Entity {
 public:
  Mesh() {}
  explicit Mesh(std::shared_ptr<SubMesh> sub_mesh);
  virtual ~Mesh() {}
  virtual void Bind() {}
  virtual void Draw(const Program& prog) {}

  std::vector<std::shared_ptr<SubMesh>> sub_meshes;
  std::vector<std::shared_ptr<Material>> materials;
};
}  // namespace pxl
