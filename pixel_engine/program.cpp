#include <pixel_engine/program.h>

#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <glog/logging.h>

namespace pxl {
namespace {
const int kBufferSize = 255;
}

Program::Program(const boost::filesystem::path& vertex_shader,
                 const boost::filesystem::path& fragment_shader) {
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
    LOG(FATAL) << info_log.data();

    // Cleanup
    glDeleteProgram(prog_id_);
    glDeleteShader(vertex_id_);
    glDeleteShader(fragment_id_);
  }
  LoadAttributes();
  LoadUniforms();
}

Program::Program(const boost::filesystem::path& compute_shader) {
  compute_id_ = LoadShader(compute_shader, GL_COMPUTE_SHADER);

  prog_id_ = glCreateProgram();
  glAttachShader(prog_id_, compute_id_);

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
    LOG(FATAL) << info_log.data();

    // Cleanup
    glDeleteShader(compute_id_);
  }
  LoadAttributes();
  LoadUniforms();
}

Program::~Program() {
  glDeleteProgram(prog_id_);
  glDeleteShader(vertex_id_);
  glDeleteShader(fragment_id_);
  glDeleteShader(compute_id_);
}

void Program::Bind() { glUseProgram(prog_id_); }

void Program::UnBind() { glUseProgram(0); }

GLint Program::GetAttributeLocation(const std::string& name) const {
  if (attributes_.count(name) != 1) {
    LOG(WARNING) << "No active attribute found with name " << name;
    return -1;
  }
  return attributes_.at(name);
}

GLint Program::GetUniformLocation(const std::string& name) const {
  if (uniforms_.count(name) != 1) {
    LOG(WARNING) << "No active uniform found with name " << name;
    return -1;
  }
  return uniforms_.at(name);
}

std::string Program::LoadFile(const boost::filesystem::path& file_path) const {
  // Load shader into a string
  std::stringstream shader_code;
  std::ifstream shader_file;
  shader_file.open(file_path.native());
  if (shader_file.is_open()) {
    std::string line;
    while (std::getline(shader_file, line)) {
      shader_code << line << std::endl;
    }
  } else {
    LOG(ERROR) << "Could not open " << file_path.string();
    return "";
  }
  return shader_code.str();
}

GLuint Program::LoadShader(const boost::filesystem::path& shader_path,
                           GLenum shader_type) {
  // Load shader into a string
  std::string shader_code_string = LoadFile(shader_path);

  std::regex include_regex("#include \"(.*)\"\n");

  std::smatch match;
  while (std::regex_search(shader_code_string, match, include_regex)) {
    std::string include_string =
        LoadFile(shader_path.parent_path() / match[1].str());
    shader_code_string =
        std::regex_replace(shader_code_string, include_regex, include_string);
  }

  // Bind Shader to GPU
  auto shader_id = glCreateShader(shader_type);
  const GLchar* data = shader_code_string.data();
  GLint code_size = shader_code_string.size() + 1;
  glShaderSource(shader_id, 1, &data, &code_size);
  glCompileShader(shader_id);

  // Error handling
  GLint success = 0;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
  if (success == GL_FALSE) {
    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);
    std::vector<GLchar> log_string(log_length);
    glGetShaderInfoLog(shader_id, log_length, NULL, log_string.data());
    LOG(FATAL) << shader_path << ":" << log_string.data();
    glDeleteShader(shader_id);
    return 0;
  }
  return shader_id;
}

void Program::LoadAttributes() {
  GLint num_active_attributes;
  glGetProgramiv(prog_id_, GL_ACTIVE_ATTRIBUTES, &num_active_attributes);
  attributes_.clear();
  for (int i = 0; i < num_active_attributes; i++) {
    std::vector<char> name_data(kBufferSize, 0);
    GLint size;
    GLenum type;
    glGetActiveAttrib(prog_id_, i, kBufferSize, NULL, &size, &type,
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
    GLint size;
    GLenum type;
    glGetActiveUniform(prog_id_, i, kBufferSize, NULL, &size, &type,
                       name_data.data());
    std::string name(name_data.data());
    uniforms_[name] = glGetUniformLocation(prog_id_, name.c_str());

    VLOG(2) << "Uniform: " << name;
  }
}

void Program::SetUniformMatrix4fv(const std::string& name,
                                  const float* data) const {
  glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, data);
}

void Program::SetUniform4fv(const std::string& name, const float* data) const {
  glUniform4fv(GetUniformLocation(name), 1, data);
}

void Program::SetUniform3fv(const std::string& name, const float* data) const {
  glUniform3fv(GetUniformLocation(name), 1, data);
}

void Program::SetUniform1f(const std::string& name, float data) const {
  glUniform1f(GetUniformLocation(name), data);
}

void Program::SetUniform1i(const std::string& name, int data) const {
  glUniform1i(GetUniformLocation(name), data);
}
}  // namespace pxl
