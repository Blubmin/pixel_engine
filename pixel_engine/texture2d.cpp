#include <pixel_engine/texture2d.h>

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION

#include <glog/logging.h>
#include <stb/stb_image.h>

namespace pxl {
Texture2d::Texture2d() {}

Texture2d::Texture2d(float* data, int32_t width, int32_t height,
                     int32_t channels)
    : image_data(data),
      width(width),
      height(height),
      channels(channels),
      format(FLOAT) {}

Texture2d::Texture2d(const boost::filesystem::path& path) : format(FLOAT) {
  stbi_set_flip_vertically_on_load(true);
  image_data = stbi_loadf(path.string().data(), &width, &height, &channels, 0);
  if (image_data == NULL) {
    LOG(FATAL) << stbi_failure_reason() << ": " << path;
  }
}

Texture2d::Texture2d(int32_t width, int32_t height, Texture2d::Format format,
                     int32_t channels)
    : image_data(NULL),
      width(width),
      height(height),
      channels(channels),
      format(format) {}

int32_t Texture2d::GetWidth() const { return width; }

int32_t Texture2d::GetHeight() const { return height; }

int32_t Texture2d::GetChannels() const { return channels; }

Texture2d::Format Texture2d::GetFormat() const { return format; }

void Texture2d::Free() { stbi_image_free(image_data); }
}  // namespace pxl
