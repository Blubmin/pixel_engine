#include <pixel_engine/ogl_texture2d.h>

#include <glog/logging.h>

#include <pixel_engine/ogl_utilities.h>

namespace pxl {
namespace {
GLenum GetInternalFormat(const Texture2d& texture) {
  int32_t channels = texture.GetChannels();
  switch (texture.GetFormat()) {
    case (Texture2d::Format::BYTE):
      if (channels == 1) {
        return GL_RED;
      } else if (channels == 3) {
        return GL_RGB;
      } else if (channels == 4) {
        return GL_RGBA;
      }
      break;
    case (Texture2d::Format::FLOAT):
      if (channels == 1) {
        return GL_R32F;
      } else if (channels == 3) {
        return GL_RGB32F;
      } else if (channels == 4) {
        return GL_RGBA32F;
      }
      break;
    case (Texture2d::Format::DEPTH_STENCIL):
      return GL_DEPTH24_STENCIL8;
      break;
    default:
      return 0;
  }
}

GLenum GetDataFormat(const Texture2d texture) {
  int32_t channels = texture.GetChannels();
  switch (texture.GetFormat()) {
    case (Texture2d::Format::BYTE):
      if (channels == 1) {
        return GL_RED;
      } else if (channels == 3) {
        return GL_RGB;
      } else if (channels == 4) {
        return GL_RGBA;
      }
      break;
    case (Texture2d::Format::FLOAT):
      if (channels == 1) {
        return GL_RED;
      } else if (channels == 3) {
        return GL_RGB;
      } else if (channels == 4) {
        return GL_RGBA;
      }
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
OglTexture2d::OglTexture2d(int32_t width, int32_t height,
                           Texture2d::Format format)
    : Texture2d(width, height, format) {}

OglTexture2d::OglTexture2d(float* data, int32_t width, int32_t height,
                           int32_t channels)
    : Texture2d(data, width, height, channels) {}

OglTexture2d::OglTexture2d(const boost::filesystem::path& path)
    : Texture2d(path) {}

OglTexture2d::~OglTexture2d() { glDeleteTextures(1, &texture_id); }

void OglTexture2d::Bind() {
  // Generate texture
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);

  // Set data
  glTexImage2D(GL_TEXTURE_2D, 0, GetInternalFormat(*this), width, height, 0,
               GetDataFormat(*this), GetDataType(format), image_data);
  glGenerateMipmap(GL_TEXTURE_2D);

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

void OglTexture2d::SetFilterMode(GLenum filter_mode) const {
  glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, filter_mode);
  if (filter_mode == GL_LINEAR_MIPMAP_LINEAR) {
    filter_mode = GL_LINEAR;
  }
  glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, filter_mode);
}

void OglTexture2d::SetWrapMode(GLenum wrap_mode) const {
  glTextureParameteri(texture_id, GL_TEXTURE_WRAP_S, wrap_mode);
  glTextureParameteri(texture_id, GL_TEXTURE_WRAP_T, wrap_mode);
}
}  // namespace pxl
