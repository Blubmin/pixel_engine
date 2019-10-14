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

  void SetClearColor(float r, float g, float b, float a = 1);

  void Begin();
  void End();
  void Push();
  void Pop();
  void BlitDepth(const OglFramebuffer& src_framebuffer);
  Eigen::Vector4f ReadPixel(GLuint x, GLuint y, uint32_t color_attachment = 0);

  std::shared_ptr<OglTexture2d> GetColorAttachment(uint32_t attachment) const;

 private:
  GLuint framebuffer_id_;
  uint32_t width_;
  uint32_t height_;
  std::map<uint32_t, std::shared_ptr<OglTexture2d>> color_attachments_;
  std::shared_ptr<OglTexture2d> depth_stencil_attachment_;
  std::vector<float> clear_color_;

  GLint prev_framebuffer_id_;
  int32_t prev_viewport_[4];
  std::vector<float> prev_clear_color_;
};
}  // namespace pxl
