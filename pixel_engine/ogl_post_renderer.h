#pragma once

#include <GL/gl3w.h>
#include <glog/logging.h>
#include <boost/filesystem.hpp>

#include <pixel_engine/program.h>

namespace pxl {
class OglPostRenderer {
 protected:
  OglPostRenderer(const boost::filesystem::path& frag_shader);
  void DrawQuad();
  std::shared_ptr<Program> GetProgram() const;

 private:
  std::shared_ptr<Program> prog_;
  void InitProg(const boost::filesystem::path& frag_shader);

  static void InitQuad();
  static GLuint vao_;
  static GLuint buffer_;
};
}  // namespace pxl
