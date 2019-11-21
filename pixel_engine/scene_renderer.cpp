#include <pixel_engine/scene_renderer.h>

#define _USE_MATH_DEFINES
#include <math.h>
#include <array>

#include <glog/logging.h>
#include <boost/format.hpp>

#include <pixel_engine/directional_light.h>
#include <pixel_engine/eigen_utilities.h>
#include <pixel_engine/game.h>
#include <pixel_engine/ogl_mesh.h>
#include <pixel_engine/ogl_ssao_renderer.h>
#include <pixel_engine/ogl_utilities.h>
#include <pixel_engine/point_light.h>
#include <pixel_engine/utilities.h>

namespace pxl {
namespace {
Eigen::Matrix4f GetTbnMatrix(Eigen::Vector3f normal) {
  normal.normalize();
  Eigen::Vector3f tangent = Eigen::Vector3f::UnitY().cross(normal);
  Eigen::Vector3f bitangent = normal.cross(tangent);
  Eigen::Matrix4f tbn = Eigen::Matrix4f::Identity();
  tbn.block<3, 1>(0, 0) = tangent;
  tbn.block<3, 1>(0, 1) = bitangent;
  tbn.block<3, 1>(0, 2) = normal;
  return tbn;
}

std::array<float, 6> GetShadowExtents(const Eigen::Vector3f& light_dir,
                                      const Camera& camera) {
  std::array<float, 6> extents;

  auto frustum_pts = camera.GetFrustumPoints();
  for (auto& pt : frustum_pts) {
    auto pt_world =
        camera.GetTransform() * Eigen::Vector4f(pt.x(), pt.y(), pt.z(), 1);
    pt = Eigen::Vector3f(pt_world.x(), pt_world.y(), pt_world.z());
  }
  auto tbn = Eigen::GetRotation(GetTbnMatrix(-light_dir)).inverse().eval();

  Eigen::Vector3f pt0 = tbn * frustum_pts[0];
  Eigen::Vector3f pt1 = tbn * frustum_pts[1];
  Eigen::Vector3f pt2 = tbn * frustum_pts[2];
  Eigen::Vector3f pt3 = tbn * frustum_pts[3];
  Eigen::Vector3f pt4 = tbn * frustum_pts[4];
  Eigen::Vector3f pt5 = tbn * frustum_pts[5];
  Eigen::Vector3f pt6 = tbn * frustum_pts[6];
  Eigen::Vector3f pt7 = tbn * frustum_pts[7];

  auto min = Eigen::Min(
      Eigen::Min(
          Eigen::Min(
              Eigen::Min(Eigen::Min(Eigen::Min(Eigen::Min(pt0, pt1), pt2), pt3),
                         pt4),
              pt5),
          pt6),
      pt7);
  auto max = Eigen::Max(
      Eigen::Max(
          Eigen::Max(
              Eigen::Max(Eigen::Max(Eigen::Max(Eigen::Max(pt0, pt1), pt2), pt3),
                         pt4),
              pt5),
          pt6),
      pt7);

  extents[0] = min.x();
  extents[1] = min.y();
  extents[2] = min.z();
  extents[3] = max.x();
  extents[4] = max.y();
  extents[5] = max.z();
  return extents;
}

Eigen::Matrix4f GetShadowProjection(const Eigen::Vector3f& light_dir,
                                    const Camera& camera) {
  auto extents = GetShadowExtents(light_dir, camera);
  return GetOrthoProjection(extents[0], extents[3], extents[1], extents[4],
                            -extents[5], -extents[2]);
}
}  // namespace

std::shared_ptr<OglFramebuffer> SceneRenderer::g_buffer_(nullptr);

std::shared_ptr<OglFramebuffer> SceneRenderer::shadow_buffer_(nullptr);

std::shared_ptr<OglFramebuffer> SceneRenderer::ssao_buffer_(nullptr);

std::shared_ptr<Program> SceneRenderer::mesh_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::grid_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::pose_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::skybox_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::shadow_map_prog_(nullptr);

std::shared_ptr<Program> SceneRenderer::deferred_prog_(nullptr);

std::shared_ptr<OglPostRenderer> SceneRenderer::deferred_lighting_prog_(
    nullptr);

void SceneRenderer::RenderScene(const Scene& scene,
                                std::shared_ptr<OglFramebuffer> dst_framebuffer,
                                float gamma) {
  if (grid_prog_ == nullptr) {
    Init();
  }

  if (scene.camera == nullptr) {
    LOG(ERROR) << "No camera set for scene.";
    return;
  }

  // Draw meshes
  GLint output_framebuffer;
  g_buffer_->Begin();
  RenderGBuffers(scene, gamma);
  g_buffer_->End();

  // Draw ssao
  ssao_buffer_->Begin();
  OglSsaoRenderer::GetInstance()->RenderTexture(*g_buffer_, *scene.camera);
  ssao_buffer_->End();

  // Draw shadow map
  shadow_buffer_->Begin();
  RenderShadow(scene);
  shadow_buffer_->End();

  // Draw deferred lighting
  dst_framebuffer->Begin();
  RenderDeferredLighting(scene, gamma);
  dst_framebuffer->BlitDepth(*g_buffer_);

  // Draw Skybox
  skybox_prog_->Bind();
  if (scene.skybox != nullptr) {
    scene.skybox->position = Eigen::GetPosition(scene.camera->GetTransform());
    scene.skybox->scale = Eigen::Vector3f::Ones() *
                          (scene.camera->far_plane - FLT_EPSILON) /
                          std::sqrt(3);
    skybox_prog_->SetUniformMatrix4fv("u_perspective",
                                      scene.camera->GetPerspective().data());
    skybox_prog_->SetUniformMatrix4fv("u_view", scene.camera->GetView().data());
    skybox_prog_->SetUniformMatrix4fv("u_model",
                                      scene.skybox->GetTransform().data());
    skybox_prog_->SetUniform1f("u_gamma", gamma);
    scene.skybox->mesh->Draw(*skybox_prog_);
  }
  skybox_prog_->UnBind();

  if (scene.debug_draw) {
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
    scene.DrawPose();
    pose_prog_->UnBind();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  }
}

void SceneRenderer::RenderGBuffers(const Scene& scene, float gamma) {
  auto meshes = scene.GetEntities<MeshEntity>();
  auto cameras = scene.GetEntities<Camera>();

  deferred_prog_->Bind();
  deferred_prog_->SetUniform1f("u_gamma", gamma);
  deferred_prog_->SetUniformMatrix4fv("u_perspective",
                                      scene.camera->GetPerspective().data());
  deferred_prog_->SetUniformMatrix4fv("u_view", scene.camera->GetView().data());

  // Draw Meshes
  for (auto mesh : meshes) {
    deferred_prog_->SetUniformMatrix4fv("u_model", mesh->GetTransform().data());
    mesh->Draw(*deferred_prog_);
  }

  // Draw Camera Meshes
  for (auto camera : cameras) {
    if (scene.camera == camera) {
      continue;
    }
    deferred_prog_->SetUniformMatrix4fv("u_model",
                                        camera->GetTransform().data());
    camera->Draw(*deferred_prog_);
  }

  deferred_prog_->UnBind();
}

void SceneRenderer::RenderShadow(const Scene& scene) {
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);

  auto meshes = scene.GetEntities<MeshEntity>();
  auto cameras = scene.GetEntities<Camera>();

  auto dir_light = scene.GetEntity<DirectionalLight>();
  auto light_dir = dir_light->direction.normalized();

  auto tbn = GetTbnMatrix(-light_dir);

  // auto ortho = GetShadowProjection(light_dir, *scene.camera);
  auto ortho = GetOrthoProjection(-40, 40, -40, 40, -100, 100);

  shadow_map_prog_->Bind();
  shadow_map_prog_->SetUniformMatrix4fv("u_perspective", ortho.data());
  shadow_map_prog_->SetUniformMatrix4fv("u_view", tbn.inverse().eval().data());

  // Draw Meshes
  for (auto mesh : meshes) {
    shadow_map_prog_->SetUniformMatrix4fv("u_model",
                                          mesh->GetTransform().data());
    for (auto ogl_mesh : mesh->GetComponents<OglMesh>()) {
      ogl_mesh->draw_materials = false;
    }
    mesh->Draw(*shadow_map_prog_);
    for (auto ogl_mesh : mesh->GetComponents<OglMesh>()) {
      ogl_mesh->draw_materials = true;
    }
  }

  // Draw Camera Meshes
  for (auto camera : cameras) {
    if (scene.camera == camera) {
      continue;
    }
    shadow_map_prog_->SetUniformMatrix4fv("u_model",
                                          camera->GetTransform().data());
    std::dynamic_pointer_cast<OglMesh>(camera->mesh->mesh)->draw_materials =
        false;
    camera->Draw(*shadow_map_prog_);
    std::dynamic_pointer_cast<OglMesh>(camera->mesh->mesh)->draw_materials =
        true;
  }

  shadow_map_prog_->UnBind();

  glCullFace(GL_BACK);
  glDisable(GL_CULL_FACE);
}

void SceneRenderer::RenderDeferredLighting(const Scene& scene, float gamma) {
  auto prog = deferred_lighting_prog_->GetProgram();
  auto point_lights = scene.GetEntities<PointLight>();
  auto dir_lights = scene.GetEntities<DirectionalLight>();

  prog->Bind();

  prog->SetUniform1f("u_gamma", gamma);
  prog->SetUniform3fv("u_camera_pos",
                      scene.camera->GetTransform().block<3, 1>(0, 3).data());
  prog->SetUniform1i("u_num_point_lights", point_lights.size());

  // Point lights
  for (size_t i = 0; i < point_lights.size(); ++i) {
    auto point_light = point_lights.at(i);
    boost::format array_format("u_point_lights[%u].");
    array_format % i;
    prog->SetUniform3fv(array_format.str() + "pos",
                        point_light->GetTransform().block<3, 1>(0, 3).data());
    prog->SetUniform3fv(array_format.str() + "color",
                        point_light->color.Data());
    prog->SetUniform1f(array_format.str() + "linear_attenuation",
                       point_light->linear_attenuation);
    prog->SetUniform1f(array_format.str() + "quadratic_attenuation",
                       point_light->quadratic_attenuation);
  }

  // Only supports one directional light at the moment;
  if (!dir_lights.empty()) {
    auto dir_light = dir_lights[0];
    prog->SetUniform3fv("u_dir_light.direction", dir_light->direction.data());
    prog->SetUniform1f("u_dir_light.strength", dir_light->strength);
    prog->SetUniform3fv("u_dir_light.color", dir_light->color.data());
  } else {
    prog->SetUniform1f("u_dir_light.strength", 0);
  }

  g_buffer_->GetColorAttachment(0)->SetSampler2D(*prog, "u_albedo_spec_texture",
                                                 0);
  g_buffer_->GetColorAttachment(1)->SetSampler2D(*prog, "u_position_texture",
                                                 1);
  g_buffer_->GetColorAttachment(2)->SetSampler2D(*prog, "u_normal_texture", 2);
  ssao_buffer_->GetColorAttachment(0)->SetSampler2D(*prog, "u_ssao_texture", 3);
  shadow_buffer_->GetColorAttachment(0)->SetSampler2D(*prog, "u_shadow_texture",
                                                      4);

  auto tbn = GetTbnMatrix(-scene.GetEntity<DirectionalLight>()->direction);
  prog->SetUniformMatrix4fv("u_shadow", tbn.inverse().eval().data());
  // Eigen::Matrix4f ortho =
  //     GetShadowProjection(dir_lights[0]->direction, *scene.camera);
  auto ortho = GetOrthoProjection(-40, 40, -40, 40, -100, 100);
  prog->SetUniformMatrix4fv("u_map", ortho.data());

  deferred_lighting_prog_->DrawQuad();
  prog->UnBind();
}

void SceneRenderer::Init() {
  glDisable(GL_BLEND);
  g_buffer_ = std::make_shared<OglFramebuffer>(1920, 1080, 4);
  g_buffer_->SetAttachmentClearColor(1, FLT_MAX, FLT_MAX, FLT_MAX);
  g_buffer_->Bind();
  GLint tex_size = 0;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &tex_size);
  shadow_buffer_ = std::make_shared<OglFramebuffer>(1 << 11, 1 << 11,
                                                    std::vector<int32_t>({1}));
  shadow_buffer_->Bind();
  shadow_buffer_->SetClearColor(FLT_MAX, FLT_MAX, FLT_MAX);
  shadow_buffer_->GetColorAttachment(0)->SetWrapMode(GL_CLAMP_TO_BORDER);
  shadow_buffer_->GetColorAttachment(0)->SetBorder(FLT_MAX, 1, 1);
  shadow_buffer_->GetColorAttachment(0)->SetFilterMode(GL_LINEAR);
  ssao_buffer_ =
      std::make_shared<OglFramebuffer>(1920, 1080, std::vector<int32_t>({1}));
  ssao_buffer_->Bind();
  mesh_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "mesh.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "mesh.frag");
  grid_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "grid.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "grid.frag");
  pose_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "pose.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "pose.frag");
  skybox_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "skybox.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "skybox.frag");
  shadow_map_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "shadow.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "shadow.frag");
  deferred_prog_ = std::make_shared<Program>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" / "mesh.vert",
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
          "mesh_deferred.frag");
  deferred_lighting_prog_ = std::make_shared<OglPostRenderer>(
      boost::filesystem::path(__FILE__).parent_path() / "shaders" /
      "deferred_lighting.frag");
}
}  // namespace pxl
