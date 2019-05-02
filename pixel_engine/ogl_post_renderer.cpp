#include <pixel_engine/ogl_post_renderer.h>

#include <GL/gl3w.h>
#include <Eigen/Core>

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

void OglPostRenderer::DrawQuad() {
  if (vao_ == 0) {
    InitQuad();
  }
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
