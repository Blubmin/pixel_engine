#pragma once

#include <map>
#include <string>

#include <GL/gl3w.h>
#include <boost/filesystem.hpp>

namespace pxl {
class Program {
 public:
  Program(const boost::filesystem::path& vertex_shader,
          const boost::filesystem::path& fragment_shader);
  Program(const boost::filesystem::path& compute_shader);
  ~Program();

  void Bind();
  void UnBind();

  GLint GetAttributeLocation(const std::string& name);
  GLint GetUniformLocation(const std::string& name);

  void SetUniformMatrix4fv(const std::string& name, const float* data);
  void SetUniform3fv(const std::string& name, const float* data);
  void SetUniform1f(const std::string& name, float data);
  void SetUniform1i(const std::string& name, int data);

 private:
  GLuint prog_id_;
  GLuint vertex_id_;
  GLuint fragment_id_;
  GLuint compute_id_;

  std::map<std::string, GLuint> attributes_;
  std::map<std::string, GLuint> uniforms_;

  GLuint LoadShader(const boost::filesystem::path& shader_path, GLenum shader);
  void LoadAttributes();
  void LoadUniforms();
};
}  // namespace pxl
