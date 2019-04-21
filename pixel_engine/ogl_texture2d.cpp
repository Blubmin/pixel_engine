#include <pixel_engine/ogl_texture2d.h>

#include <glog/logging.h>

#include <pixel_engine/ogl_utilities.h>

namespace pxl {
namespace {
GLenum GetInternalFormat(Texture2d::Format format) {
  switch (format) {
    case (Texture2d::Format::BYTE):
      return GL_RGBA;
      break;
    case (Texture2d::Format::FLOAT):
      return GL_RGBA32F;
      break;
    case (Texture2d::Format::DEPTH_STENCIL):
      return GL_DEPTH24_STENCIL8;
      break;
    default:
      return 0;
  }
}

GLenum GetDataFormat(Texture2d::Format format) {
  switch (format) {
    case (Texture2d::Format::BYTE):
      return GL_RGBA;
      break;
    case (Texture2d::Format::FLOAT):
      return GL_RGBA;
      break;
    case (Texture2d::Format::DEPTH_STENCIL):
      return GL_DEPTH_STENCIL;
      break;
    default:
      return 0;
  }
}

GLenum GetDataType(Texture2d::Format format) {
  switch (format) {
    case (Texture2d::Format::BYTE):
      return GL_UNSIGNED_BYTE;
      break;
    case (Texture2d::Format::FLOAT):
      return GL_FLOAT;
      break;
    case (Texture2d::Format::DEPTH_STENCIL):
      return GL_UNSIGNED_INT_24_8;
      break;
    default:
      return 0;
  }
}
}  // namespace
OglTexture2d::OglTexture2d(uint32_t width, uint32_t height,
                           Texture2d::Format format)
    : Texture2d(width, height, format) {}

OglTexture2d::~OglTexture2d() { glDeleteTextures(1, &texture_id); }

void OglTexture2d::Bind() {
  // Generate texture
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Set data
  glTexImage2D(GL_TEXTURE_2D, 0, GetInternalFormat(format), width, height, 0,
               GetDataFormat(format), GetDataType(format), 0);

  // Set parameters
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Clear
  glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint OglTexture2d::GetTextureId() const { return texture_id; }

void OglTexture2d::Use(uint32_t texture_unit) const {
  glActiveTexture(GL_TEXTURE0 + texture_unit);
  glBindTexture(GL_TEXTURE_2D, texture_id);
}
}  // namespace pxl
