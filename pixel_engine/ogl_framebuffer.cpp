#include <pixel_engine/ogl_framebuffer.h>

#include <glog/logging.h>

#include <pixel_engine/game.h>
#include <pixel_engine/ogl_utilities.h>

namespace pxl {
OglFramebuffer::OglFramebuffer(uint32_t width, uint32_t height,
                               uint32_t color_attachments)
    : width_(width), height_(height) {
  for (uint32_t i = 0; i < color_attachments; ++i) {
    color_attachments_.emplace(
        i, std::make_shared<OglTexture2d>(width_, height_, Texture2d::FLOAT));
  }
  depth_stencil_attachment_ =
      std::make_shared<OglTexture2d>(width_, height_, Texture2d::DEPTH_STENCIL);
}

OglFramebuffer::~OglFramebuffer() { glDeleteFramebuffers(1, &framebuffer_id_); }

void OglFramebuffer::Bind() {
  // Create
  glGenFramebuffers(1, &framebuffer_id_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);

  // Color attachments
  std::vector<GLuint> attachments;
  for (auto& pair : color_attachments_) {
    const auto& attachment = pair.first;
    auto& texture = pair.second;
    texture->Bind();

    glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment,
                           GL_TEXTURE_2D, texture->GetTextureId(), 0);
    attachments.emplace_back(GL_COLOR_ATTACHMENT0 + attachment);
  }
  glDrawBuffers(attachments.size(), attachments.data());

  // Depth stencil
  depth_stencil_attachment_->Bind();
  glBindTexture(GL_TEXTURE_2D, depth_stencil_attachment_->GetTextureId());
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         GL_TEXTURE_2D,
                         depth_stencil_attachment_->GetTextureId(), 0);

  // Cleanup
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void OglFramebuffer::Start() {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
  glViewport(0, 0, width_, height_);
  glClearColor(0.15f, 0.15f, 0.15f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OglFramebuffer::End() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(.15f, .15f, .15f, 1.f);
  glViewport(0, 0, Game::State.window_width, Game::State.window_height);
}

void OglFramebuffer::BlitDepth(const OglFramebuffer& src_framebuffer) {
  glBlitNamedFramebuffer(src_framebuffer.framebuffer_id_, framebuffer_id_, 0, 0,
                         src_framebuffer.width_, src_framebuffer.height_, 0, 0,
                         width_, height_, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

std::shared_ptr<OglTexture2d> OglFramebuffer::GetColorAttachment(
    uint32_t attachment) const {
  return color_attachments_.at(attachment);
}

Eigen::Vector4f OglFramebuffer::ReadPixel(GLuint x, GLuint y,
                                          uint32_t color_attachment) {
  GLint prev_framebuffer, prev_read_buffer;
  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prev_framebuffer);
  glGetIntegerv(GL_READ_BUFFER, &prev_read_buffer);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer_id_);
  glReadBuffer(GL_COLOR_ATTACHMENT0 + color_attachment);

  Eigen::Vector4f pixel;
  glReadPixels(x, y, 1, 1, GL_RGBA, GL_FLOAT, pixel.data());
  glReadBuffer(prev_read_buffer);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, prev_framebuffer);
  return pixel;
}
}  // namespace pxl
