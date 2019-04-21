#pragma once

#include <vector>

#include <Eigen/Core>

#include <pixel_engine/ogl_texture2d.h>
#include <pixel_engine/program.h>

namespace pxl {
class OglTextureRenderer {
 public:
  static void RenderTexture(const OglTexture2d& texture);

 private:
  static void Init();

  static std::shared_ptr<Program> prog_;
  static GLuint vao_;
  static GLuint buffer_;
  static std::vector<Eigen::Vector2f> positions_;
};
}  // namespace pxl
