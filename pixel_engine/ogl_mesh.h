#pragma once

#include <GL/gl3w.h>
#include <pixel_engine/mesh.h>

namespace pxl {
class OglMesh : public Mesh {
 public:
  ~OglMesh() override;
  void Bind() override;
  void Draw() override;

  GLuint vao;
  std::vector<GLuint> vaos;
  std::vector<std::vector<GLuint>> buffers;
};
}  // namespace pxl
