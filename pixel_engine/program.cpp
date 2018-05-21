#include <pixel_engine/program.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace pixel_engine {
Program::Program(const std::experimental::filesystem::path& vertex_shader,
                 const std::experimental::filesystem::path& fragment_shader) {
  GLuint vertex_id = LoadShader(vertex_shader, GL_VERTEX_SHADER);
  GLuint fragment_id = LoadShader(fragment_shader, GL_FRAGMENT_SHADER);

  prog_id_ = glCreateProgram();
  glAttachShader(prog_id_, vertex_id);
  glAttachShader(prog_id_, fragment_id);

  // Error checking
  glLinkProgram(prog_id_);
  GLint is_linked = 0;
  glGetProgramiv(prog_id_, GL_LINK_STATUS, &is_linked);
  if (is_linked == GL_FALSE) {
    // Get error
    GLint max_length = 0;
    glGetProgramiv(prog_id_, GL_INFO_LOG_LENGTH, &max_length);
    std::vector<GLchar> info_log(max_length);
    glGetProgramInfoLog(prog_id_, max_length, &max_length, info_log.data());
    std::cerr << info_log.data();

    // Cleanup
    glDeleteProgram(prog_id_);
    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
  }
}

void Program::Bind() { glUseProgram(prog_id_); }

void Program::UnBind() { glUseProgram(0); }

GLuint Program::LoadShader(
    const std::experimental::filesystem::path& shader_path,
    GLenum shader_type) {
  // Load shader into a string
  std::ostringstream shader_code;
  std::ifstream shader_file;
  shader_file.open(shader_path.native());
  if (shader_file.is_open()) {
    std::string line;
    while (std::getline(shader_file, line)) {
      shader_code << line;
    }
  } else {
    std::cerr << "Could not open " << shader_path.string();
    return 0;
  }

  // Bind Shader to GPU
  auto shader_id = glCreateShader(shader_type);
  const char* shader_code_string = shader_code.str().c_str();
  GLint code_size = strlen(shader_code_string);
  glShaderSource(shader_id, 1, &shader_code_string, &code_size);
  glCompileShader(shader_id);

  // Error handling
  GLint success = 0;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<GLchar> log_string(log_length);
    glGetShaderInfoLog(shader_id, log_length, NULL, log_string.data());
    std::cerr << log_string.data();
    glDeleteShader(shader_id);
    return 0;
  }

  return shader_id;
}
}  // namespace pixel_engine
