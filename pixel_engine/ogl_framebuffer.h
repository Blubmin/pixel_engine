#pragma once

#include <cstdint>
#include <map>
#include <memory>

#include <GL/gl3w.h>
#include <Eigen/Core>

#include <pixel_engine/ogl_texture2d.h>

namespace pxl {
class OglFramebuffer {
 public:
  OglFramebuffer(uint32_t width, uint32_t height,
                 uint32_t color_attachments = 1);
  ~OglFramebuffer();

  void Bind();

  void Start();
  void End();
  void BlitDepth(const OglFramebuffer& src_framebuffer);
  Eigen::Vector4f ReadPixel(GLuint x, GLuint y, uint32_t color_attachment = 0);

  std::shared_ptr<OglTexture2d> GetColorAttachment(uint32_t attachment) const;

 private:
  GLuint framebuffer_id_;
  uint32_t width_;
  uint32_t height_;
  std::map<uint32_t, std::shared_ptr<OglTexture2d>> color_attachments_;
  std::shared_ptr<OglTexture2d> depth_stencil_attachment_;
};
}  // namespace pxl
