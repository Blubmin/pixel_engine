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
  std::vector<float> tangents;
  std::vector<float> texture_coordinates;
  uint32_t material_idx;

  void ComputeNormalsAndTangents();
  void ComputeTangents();
};

class Mesh : public Component {
 public:
  Mesh() {}
  explicit Mesh(std::shared_ptr<SubMesh> sub_mesh);
  virtual ~Mesh() {}
  virtual void Bind() {}
  void Draw(const Program& prog) override {}

  std::vector<std::shared_ptr<SubMesh>> sub_meshes;
  std::vector<std::shared_ptr<Material>> materials;
};

class MeshEntity : public Entity {
 public:
  MeshEntity(std::shared_ptr<pxl::Mesh> mesh);
  void Bind();
  void Draw(const Program& prog);

  std::shared_ptr<pxl::Mesh> mesh;
};
}  // namespace pxl
