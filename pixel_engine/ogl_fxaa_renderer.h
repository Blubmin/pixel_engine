#pragma once

#include <pixel_engine/ogl_post_renderer.h>

#include <Eigen/Core>

#include <pixel_engine/ogl_texture2d.h>

namespace pxl {
class OglFxaaRenderer : public OglPostRenderer {
 public:
  OglFxaaRenderer();
  static std::shared_ptr<OglFxaaRenderer> GetInstance();
  void RenderTexture(const OglTexture2d& in_texture);
};
}  // namespace pxl
