#include <pixel_engine/scene_renderer.h>

#include <glog/logging.h>

#include <pixel_engine/utilities.h>

namespace pxl {
std::shared_ptr<Program> SceneRenderer::grid_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::pose_prog_(nullptr);

void SceneRenderer::RenderScene(const Scene& scene) {
  if (grid_prog_ == nullptr) {
    Init();
  }

  if (scene.camera == nullptr) {
    LOG(ERROR) << "No camera set for scene.";
    return;
  }

  // Draw grid
  grid_prog_->Bind();
  grid_prog_->SetUniformMatrix4fv("u_perspective",
                                  scene.camera->GetPerspective().data());
  grid_prog_->SetUniformMatrix4fv("u_view", scene.camera->GetView().data());
  scene.DrawGrid();
  grid_prog_->UnBind();

  // Draw pose
  float viewport[4];
  glGetFloatv(GL_VIEWPORT, viewport);
  glViewport(0, 0, 100, 100);
  pose_prog_->Bind();
  auto view_matrix = scene.camera->GetView();
  view_matrix.block<3, 1>(0, 3) << 0.f, 0.f, 0.f;
  pose_prog_->SetUniformMatrix4fv("u_view", view_matrix.data());
  glDisable(GL_DEPTH);
  glDepthMask(0x00);
  scene.DrawPose();
  glEnable(GL_DEPTH);
  glDepthMask(0xff);
  pose_prog_->UnBind();
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

void SceneRenderer::Init() {
  grid_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "grid.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "grid.frag");
  pose_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "pose.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "pose.frag");
}
}  // namespace pxl
