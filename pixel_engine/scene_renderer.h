#pragma once

#include <memory>

#include <pixel_engine/program.h>
#include <pixel_engine/scene.h>

namespace pxl {
class SceneRenderer {
 public:
  static void RenderScene(const Scene& scene, float gamma = 2.2f);

 private:
  static void RenderMeshes(const Scene& scene, float gamma);
  static void Init();

  static std::shared_ptr<Program> mesh_prog_;
  static std::shared_ptr<Program> grid_prog_;
  static std::shared_ptr<Program> pose_prog_;
  static std::shared_ptr<Program> skybox_prog_;
};
}  // namespace pxl
