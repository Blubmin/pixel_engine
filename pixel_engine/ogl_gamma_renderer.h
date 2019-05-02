#pragma once

#include <pixel_engine/ogl_post_renderer.h>

#include <pixel_engine/ogl_texture2d.h>

namespace pxl {
class OglGammaRenderer : public OglPostRenderer {
 public:
  OglGammaRenderer();
  static std::shared_ptr<OglGammaRenderer> GetInstance();
  void OglGammaRenderer::RenderTexture(std::shared_ptr<OglTexture2d> texture,
                                       float gamma);
};
}  // namespace pxl
