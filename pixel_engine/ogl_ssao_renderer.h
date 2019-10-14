#pragma once

#include <pixel_engine/ogl_post_renderer.h>

#include <memory>
#include <vector>

#include <pixel_engine/camera.h>
#include <pixel_engine/ogl_framebuffer.h>
#include <pixel_engine/ogl_texture2d.h>

namespace pxl {
class OglSsaoRenderer : public OglPostRenderer {
 public:
  OglSsaoRenderer();
  static std::shared_ptr<OglSsaoRenderer> GetInstance();
  void RenderTexture(const OglFramebuffer& g_buffer, const Camera& camera);

 private:
  std::shared_ptr<OglFramebuffer> ssao_framebuffer_;
  std::vector<float> kernel_;
  std::shared_ptr<OglTexture2d> noise_;
};
}  // namespace pxl