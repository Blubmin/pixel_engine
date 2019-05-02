#include <pixel_engine/ogl_texture_renderer.h>

#include <glog/logging.h>

namespace pxl {
std::shared_ptr<Program> OglTextureRenderer::prog_(nullptr);
GLuint OglTextureRenderer::vao_(0);
GLuint OglTextureRenderer::buffer_(0);
std::vector<Eigen::Vector2f> OglTextureRenderer::positions_(
    {Eigen::Vector2f(-1, 1), Eigen::Vector2f(-1, -1), Eigen::Vector2f(1, 1),
     Eigen::Vector2f(1, -1)});

void OglTextureRenderer::RenderTexture(const OglTexture2d& texture) {
  if (prog_ == nullptr) {
    Init();
  }

  prog_->Bind();
  glBindVertexArray(vao_);

  texture.Use(0);
  glUniform1i(prog_->GetUniformLocation("texture_sampler"), 0);

  glDisable(GL_DEPTH);
  glDepthMask(0x00);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glDepthMask(0xff);
  glEnable(GL_DEPTH);

  glBindVertexArray(0);
  prog_->UnBind();
}

void OglTextureRenderer::Init() {
  prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "post.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "texture.frag");

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &buffer_);

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, buffer_);
  glBufferData(GL_ARRAY_BUFFER, positions_.size() * sizeof(positions_[0]),
               positions_.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
}  // namespace pxl
