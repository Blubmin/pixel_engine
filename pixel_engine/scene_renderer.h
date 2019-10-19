#pragma once

#include <memory>

#include <pixel_engine/ogl_framebuffer.h>
#include <pixel_engine/ogl_post_renderer.h>
#include <pixel_engine/program.h>
#include <pixel_engine/scene.h>

namespace pxl {
class SceneRenderer {
 public:
  static void RenderScene(const Scene& scene,
                          std::shared_ptr<OglFramebuffer> dst_framebuffer,
                          float gamma = 2.2f);

 private:
  static void RenderGBuffers(const Scene& scene, float gamma);
  static void RenderShadow(const Scene& scene);
  static void RenderDeferredLighting(const Scene& scene, float gamma);
  static void Init();

  static std::shared_ptr<Program> mesh_prog_;
  static std::shared_ptr<Program> grid_prog_;
  static std::shared_ptr<Program> pose_prog_;
  static std::shared_ptr<Program> skybox_prog_;
  static std::shared_ptr<Program> shadow_map_prog_;
  static std::shared_ptr<Program> deferred_prog_;
  static std::shared_ptr<OglPostRenderer> deferred_lighting_prog_;

 public:
  static std::shared_ptr<OglFramebuffer> g_buffer_;
  static std::shared_ptr<OglFramebuffer> shadow_buffer_;
  static std::shared_ptr<OglFramebuffer> ssao_buffer_;
};
}  // namespace pxl
