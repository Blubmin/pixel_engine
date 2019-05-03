#pragma once

#include <pixel_engine/ogl_post_renderer.h>

#include <vector>

#include <Eigen/Core>

#include <pixel_engine/ogl_texture2d.h>
#include <pixel_engine/program.h>

namespace pxl {
class OglTextureRenderer : public OglPostRenderer {
 public:
  OglTextureRenderer();
  static std::shared_ptr<OglTextureRenderer> GetInstance();
  void RenderTexture(const OglTexture2d& texture);
};
}  // namespace pxl
