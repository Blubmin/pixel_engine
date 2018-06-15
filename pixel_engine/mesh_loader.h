#pragma once

#include <filesystem>

#include <pixel_engine/mesh.h>

namespace pxl {
class MeshLoader {
 public:
  static std::shared_ptr<Mesh> LoadMesh(
      const std::experimental::filesystem::path &path);
};
}  // namespace pxl
