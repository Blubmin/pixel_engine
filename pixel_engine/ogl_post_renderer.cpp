#include <pixel_engine/ogl_post_renderer.h>

#include <GL/gl3w.h>
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace pxl {
namespace {
const std::vector<Eigen::Vector2f> kPositions({Eigen::Vector2f(-1, 1),
                                               Eigen::Vector2f(-1, -1),
                                               Eigen::Vector2f(1, 1),
                                               Eigen::Vector2f(1, -1)});
}
GLuint OglPostRenderer::vao_(0);

GLuint OglPostRenderer::buffer_(0);

OglPostRenderer::OglPostRenderer(const boost::filesystem::path& frag_shader) {
  InitProg(frag_shader);
}

void OglPostRenderer::InitProg(const boost::filesystem::path& frag_shader) {
  prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "post.vert",
      frag_shader);
}

std::shared_ptr<Program> OglPostRenderer::GetProgram() const { return prog_; }

void OglPostRenderer::DrawQuad(const Eigen::Rectf roi) {
  if (vao_ == 0) {
    InitQuad();
  }
  auto min = roi.corner(Eigen::Rectf::BottomLeft) * 2 - Eigen::Vector2f::Ones();
  auto max = roi.corner(Eigen::Rectf::TopRight) * 2 - Eigen::Vector2f::Ones();
  Eigen::Vector4f rect(min.x(), min.y(), max.x(), max.y());
  prog_->SetUniform4fv("roi", rect.data());
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void OglPostRenderer::InitQuad() {
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &buffer_);

  glBindVertexArray(vao_);

  glBindBuffer(GL_ARRAY_BUFFER, buffer_);
  glBufferData(GL_ARRAY_BUFFER, kPositions.size() * sizeof(kPositions[0]),
               kPositions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
}  // namespace pxl
