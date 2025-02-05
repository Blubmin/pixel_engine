#pragma once

#include <GL/gl3w.h>
#include <glog/logging.h>
#include <boost/filesystem.hpp>

#include <pixel_engine/eigen_utilities.h>
#include <pixel_engine/program.h>

namespace pxl {

class OglPostRenderer {
 public:
  OglPostRenderer(const boost::filesystem::path& frag_shader);
  std::shared_ptr<Program> GetProgram() const;
  void DrawQuad(const Eigen::Rectf roi = Eigen::Rectf(Eigen::Vector2f(-1, -1),
                                                      Eigen::Vector2f(1, 1)));

 private:
  std::shared_ptr<Program> prog_;
  void InitProg(const boost::filesystem::path& frag_shader);

  static void InitQuad();
  static GLuint vao_;
  static GLuint buffer_;
};
}  // namespace pxl
