#include <pixel_engine/ogl_fxaa_renderer.h>

namespace pxl {
std::shared_ptr<Program> OglFxaaRenderer::prog_(nullptr);
GLuint OglFxaaRenderer::vao_(0);
GLuint OglFxaaRenderer::buffer_(0);
std::vector<Eigen::Vector2f> OglFxaaRenderer::positions_(
    {Eigen::Vector2f(-1, 1), Eigen::Vector2f(-1, -1), Eigen::Vector2f(1, 1),
     Eigen::Vector2f(1, -1)});

void OglFxaaRenderer::RenderTexture(const OglTexture2d& in_texture) {
  if (prog_ == nullptr) {
    Init();
  }

  prog_->Bind();
  // glBindImageTexture(0, in_texture.GetTextureId(), 0, GL_FALSE, 0,
  // GL_READ_ONLY,
  //                   GL_RGBA32F);
  // glBindImageTexture(1, out_texture.GetTextureId(), 0, GL_FALSE, 0,
  //                   GL_WRITE_ONLY, GL_RGBA32F);

  // glDispatchCompute(out_texture.GetWidth(), out_texture.GetHeight(), 1);

  //// make sure writing to image has finished before read
  // glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  in_texture.Use(0);
  glUniform1i(prog_->GetUniformLocation("in_image"), 0);
  glUniform1i(prog_->GetUniformLocation("width"), in_texture.GetWidth());
  glUniform1i(prog_->GetUniformLocation("height"), in_texture.GetHeight());

  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);

  prog_->UnBind();
}

void OglFxaaRenderer::Init() {
  prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "fxaa.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "fxaa.frag");
  // prog_ = std::make_shared<Program>(
  //    boost::filesystem::path(__FILE__).parent_path() / "shaders" /
  //    "fxaa.comp");

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
