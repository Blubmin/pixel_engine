#include <pixel_engine/program.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <glog/logging.h>

namespace pxl {
namespace {
const int kBufferSize = 255;
}

Program::Program(const std::experimental::filesystem::path& vertex_shader,
                 const std::experimental::filesystem::path& fragment_shader) {
  vertex_id_ = LoadShader(vertex_shader, GL_VERTEX_SHADER);
  fragment_id_ = LoadShader(fragment_shader, GL_FRAGMENT_SHADER);

  prog_id_ = glCreateProgram();
  glAttachShader(prog_id_, vertex_id_);
  glAttachShader(prog_id_, fragment_id_);

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
    LOG(ERROR) << info_log.data();

    // Cleanup
    glDeleteProgram(prog_id_);
    glDeleteShader(vertex_id_);
    glDeleteShader(fragment_id_);
  }
}

Program::~Program() {
  glDeleteProgram(prog_id_);
  glDeleteShader(vertex_id_);
  glDeleteShader(fragment_id_);
}

void Program::Bind() { glUseProgram(prog_id_); }

void Program::UnBind() { glUseProgram(0); }

GLint Program::GetAttributeLocation(const std::string& name) {
  if (attributes_.count(name) != 1) {
    LOG(WARNING) << "No active attribute found with name " << name;
    return -1;
  }
  return attributes_.at(name);
}

GLint Program::GetUniformLocation(const std::string& name) {
  if (uniforms_.count(name) != 1) {
    LOG(WARNING) << "No active uniform found with name " << name;
    return -1;
  }
  return uniforms_.at(name);
}

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
    LOG(ERROR) << "Could not open " << shader_path.string();
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
    LOG(ERROR) << log_string.data();
    glDeleteShader(shader_id);
    return 0;
  }
  LoadAttributes();
  LoadUniforms();
  return shader_id;
}

void Program::LoadAttributes() {
  GLint num_active_attributes;
  glGetProgramiv(prog_id_, GL_ACTIVE_ATTRIBUTES, &num_active_attributes);
  attributes_.clear();
  for (int i = 0; i < num_active_attributes; i++) {
    std::vector<char> name_data(kBufferSize, 0);
    glGetActiveAttrib(prog_id_, i, kBufferSize, NULL, NULL, NULL,
                      name_data.data());
    std::string name(name_data.data());
    attributes_[name] = glGetAttribLocation(prog_id_, name.c_str());

    VLOG(2) << "Attribute: " << name;
  }
}

void Program::LoadUniforms() {
  GLint num_active_uniforms;
  glGetProgramiv(prog_id_, GL_ACTIVE_UNIFORMS, &num_active_uniforms);
  uniforms_.clear();
  for (int i = 0; i < num_active_uniforms; i++) {
    std::vector<char> name_data(kBufferSize, 0);
    glGetActiveUniform(prog_id_, i, kBufferSize, NULL, NULL, NULL,
                       name_data.data());
    std::string name(name_data.data());
    uniforms_[name] = glGetUniformLocation(prog_id_, name.c_str());

    VLOG(2) << "Uniform: " << name;
  }
}
}  // namespace pxl
