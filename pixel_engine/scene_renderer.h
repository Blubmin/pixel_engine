#pragma once

#include <memory>

#include <pixel_engine/program.h>
#include <pixel_engine/scene.h>

namespace pxl {
class SceneRenderer {
 public:
  static void RenderScene(const Scene& scene);

 private:
  static void Init();

  static std::shared_ptr<Program> grid_prog_;
  static std::shared_ptr<Program> pose_prog_;
};
}  // namespace pxl