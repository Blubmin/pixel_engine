#pragma once

#include <pixel_engine/ogl_post_renderer.h>

#include <vector>

#include <Eigen/Core>

#include <pixel_engine/eigen_utilities.h>
#include <pixel_engine/ogl_texture2d.h>
#include <pixel_engine/program.h>

namespace pxl {
class OglTextureRenderer : public OglPostRenderer {
 public:
  OglTextureRenderer();
  static std::shared_ptr<OglTextureRenderer> GetInstance();
  void RenderTexture(const OglTexture2d& texture,
                     const Eigen::Rectf roi = Eigen::Rectf(
                         Eigen::Vector2f(-1, -1), Eigen::Vector2f(1, 1)));
};
}  // namespace pxl
