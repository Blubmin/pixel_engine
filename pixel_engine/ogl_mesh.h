#pragma once

#include <GL/gl3w.h>
#include <pixel_engine/mesh.h>

#include <pixel_engine/ogl_material.h>

namespace pxl {
class OglSubMesh : public SubMesh {
 public:
  OglSubMesh();
  explicit OglSubMesh(std::shared_ptr<SubMesh> sub_mesh);
};

class OglMesh : public Mesh {
 public:
  OglMesh();
  explicit OglMesh(std::shared_ptr<SubMesh> sub_mesh);
  ~OglMesh() override;
  void Bind() override;
  void Draw(const Program& prog) override;

  GLuint vao;
  std::vector<GLuint> vaos;
  std::vector<std::vector<GLuint>> buffers;
  bool draw_materials;

 private:
  bool is_bound_;
};
}  // namespace pxl
