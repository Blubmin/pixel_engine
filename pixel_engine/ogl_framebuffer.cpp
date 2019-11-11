#include <pixel_engine/ogl_framebuffer.h>

#include <glog/logging.h>

#include <pixel_engine/game.h>
#include <pixel_engine/ogl_utilities.h>

namespace pxl {
namespace {
const std::array<float, 4> kDefaultClear = {.15f, .15f, .15f, 1.f};
}

OglFramebuffer::OglFramebuffer(uint32_t width, uint32_t height,
                               uint32_t color_attachments)
    : width_(width), height_(height), prev_clear_color_(4) {
  for (uint32_t i = 0; i < color_attachments; ++i) {
    color_attachments_.emplace(
        i, std::make_shared<OglTexture2d>(width_, height_, Texture2d::FLOAT));
    clear_colors_.emplace(i, kDefaultClear);
  }
  depth_stencil_attachment_ =
      std::make_shared<OglTexture2d>(width_, height_, Texture2d::DEPTH_STENCIL);
}

OglFramebuffer::OglFramebuffer(uint32_t width, uint32_t height,
                               const std::vector<int32_t>& attachment_channels)
    : width_(width), height_(height), prev_clear_color_(4) {
  for (uint32_t i = 0; i < attachment_channels.size(); ++i) {
    color_attachments_.emplace(
        i, std::make_shared<OglTexture2d>(width_, height_, Texture2d::FLOAT,
                                          attachment_channels[i]));
    clear_colors_.emplace(i, kDefaultClear);
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

    texture->SetWrapMode(GL_CLAMP_TO_EDGE);
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

void OglFramebuffer::SetClearColor(float r, float g, float b, float a) {
  for (auto& pair : clear_colors_) {
    auto& clear_color = pair.second;
    clear_color[0] = r;
    clear_color[1] = g;
    clear_color[2] = b;
    clear_color[3] = a;
  }
}

void OglFramebuffer::SetAttachmentClearColor(uint32_t attachment, float r,
                                             float g, float b, float a) {
  auto& clear_color = clear_colors_[attachment];
  clear_color[0] = r;
  clear_color[1] = g;
  clear_color[2] = b;
  clear_color[3] = a;
}

void OglFramebuffer::Begin() {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_id_);
  glViewport(0, 0, width_, height_);
  for (const auto pair : clear_colors_) {
    glClearBufferfv(GL_COLOR, pair.first, pair.second.data());
  }
  glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OglFramebuffer::End() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, Game::State.window_width, Game::State.window_height);
}

void OglFramebuffer::Push() {
  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prev_framebuffer_id_);
  glGetIntegerv(GL_VIEWPORT, prev_viewport_);
  glGetFloatv(GL_COLOR_CLEAR_VALUE, prev_clear_color_.data());
  Begin();
}

void OglFramebuffer::Pop() {
  End();
  glBindFramebuffer(GL_FRAMEBUFFER, prev_framebuffer_id_);
  glViewport(prev_viewport_[0], prev_viewport_[1], prev_viewport_[2],
             prev_viewport_[3]);
  glClearColor(prev_clear_color_[0], prev_clear_color_[1], prev_clear_color_[2],
               prev_clear_color_[3]);
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
