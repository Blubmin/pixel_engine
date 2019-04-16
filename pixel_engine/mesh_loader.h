#pragma once

#include <boost/filesystem.hpp>

#include <pixel_engine/mesh.h>

namespace pxl {
class MeshLoader {
 public:
  static std::shared_ptr<Mesh> LoadMesh(const boost::filesystem::path& path);
};
}  // namespace pxl
