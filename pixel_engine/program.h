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

  GLint GetAttributeLocation(const std::string& name) const;
  GLint GetUniformLocation(const std::string& name) const;

  void SetUniformMatrix4fv(const std::string& name, const float* data) const;
  void SetUniform4fv(const std::string& name, const float* data) const;
  void SetUniform3fv(const std::string& name, const float* data,
                     uint32_t count = 1) const;
  void SetUniform2fv(const std::string& name, const float* data) const;
  void SetUniform2f(const std::string& name, float v1, float v2) const;
  void SetUniform1f(const std::string& name, float data) const;
  void SetUniform1i(const std::string& name, int data) const;

 private:
  GLuint prog_id_;
  GLuint vertex_id_;
  GLuint fragment_id_;
  GLuint compute_id_;

  std::string files_;
  std::map<std::string, GLuint> attributes_;
  std::map<std::string, GLuint> uniforms_;

  std::string LoadFile(const boost::filesystem::path& file_path) const;
  GLuint LoadShader(const boost::filesystem::path& shader_path, GLenum shader);
  void LoadAttributes();
  void LoadUniforms();
};
}  // namespace pxl
