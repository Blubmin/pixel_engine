#include <pixel_engine/ogl_framebuffer.h>

#include <glog/logging.h>

#include <pixel_engine/game.h>
#include <pixel_engine/ogl_utilities.h>

namespace pxl {
OglFramebuffer::OglFramebuffer(uint32_t width, uint32_t height)
    : width_(width), height_(height) {
  color_attachments_.emplace(
      0, std::make_shared<OglTexture2d>(width_, height_, Texture2d::FLOAT));
  depth_stencil_attachment_ =
      std::make_shared<OglTexture2d>(width_, height_, Texture2d::DEPTH_STENCIL);
}

OglFramebuffer::~OglFramebuffer() { glDeleteFramebuffers(1, &framebuffer_id_); }

void OglFramebuffer::Bind() {
  // Create
  glGenFramebuffers(1, &framebuffer_id_);
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);

  // Color attachments
  for (auto& pair : color_attachments_) {
    const auto& attachment = pair.first;
    auto& texture = pair.second;
    texture->Bind();

    glBindTexture(GL_TEXTURE_2D, texture->GetTextureId());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment,
                           GL_TEXTURE_2D, texture->GetTextureId(), 0);
  }

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
  glClearColor(.3f, .3f, .3f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OglFramebuffer::End() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(.3f, .3f, .3f, 1.f);
  glViewport(0, 0, Game::State.window_width, Game::State.window_height);
}

std::shared_ptr<OglTexture2d> OglFramebuffer::GetColorAttachment(
    uint32_t attachment) const {
  return color_attachments_.at(attachment);
}
}  // namespace pxl
