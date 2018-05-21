#pragma once

#include <filesystem>

#include <GL/gl3w.h>

namespace pixel_engine {
class Program {
 public:
  Program(const std::experimental::filesystem::path& vertex_shader,
          const std::experimental::filesystem::path& fragment_shader);

  void Bind();
  void UnBind();

 private:
  GLuint prog_id_;

  GLuint LoadShader(const std::experimental::filesystem::path& shader_path,
                    GLenum shader);
};
}  // namespace pixel_engine
