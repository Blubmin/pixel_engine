#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/gl3w.h>
#include <glog/logging.h>
#include <imgui/imgui.h>
#include <pixel_engine/camera.h>
#include <pixel_engine/free_camera_component.h>
#include <pixel_engine/game.h>
#include <pixel_engine/mesh_loader.h>
#include <pixel_engine/ogl_framebuffer.h>
#include <pixel_engine/ogl_fxaa_renderer.h>
#include <pixel_engine/ogl_gamma_renderer.h>
#include <pixel_engine/ogl_mesh.h>
#include <pixel_engine/ogl_texture_renderer.h>
#include <pixel_engine/point_light.h>
#include <pixel_engine/program.h>
#include <pixel_engine/scene.h>
#include <pixel_engine/scene_renderer.h>
#include <Eigen/Geometry>
#include <boost/format.hpp>

namespace {
boost::filesystem::path GetResourcePath() {
  return boost::filesystem::path(__FILE__).parent_path() / "resources";
}
boost::filesystem::path GetMeshPath(const std::string& mesh_file) {
  return GetResourcePath() / "meshes" / mesh_file;
}
boost::filesystem::path GetShaderPath(const std::string& shader_file) {
  return GetResourcePath() / "shaders" / shader_file;
}
}  // namespace

class HelloGame : public pxl::Game {
 public:
  HelloGame() : pxl::Game("Hello Game") {}
  void Init() override {
    mesh =
        pxl::MeshLoader::LoadMeshEntity<pxl::OglMesh>(GetMeshPath("bunny.obj"));
    auto child = pxl::MeshLoader::LoadMeshEntity<pxl::OglMesh>(
        GetMeshPath("sphere.obj"));
    child->Bind();
    // child->scale = Eigen::Vector3f(.01f, .01f, .01f);
    child->position = Eigen::Vector3f(1.f, 1.f, 0.f);
    mesh->AddChild(child);
    mesh->Bind();
    // mesh->scale = Eigen::Vector3f(.01, .01, .01);
    mesh->rotation.y() = (M_PI / 2.f);

    auto child_child = pxl::MeshLoader::LoadMeshEntity<pxl::OglMesh>(
        GetMeshPath("sphere.obj"));
    CHECK(child_child != nullptr);
    // child->AddChild(child_child);

    ground =
        pxl::MeshLoader::LoadMeshEntity<pxl::OglMesh>(GetMeshPath("plane.obj"));
    ground->Bind();
    ground->position -= Eigen::Vector3f(0.f, 0.01f, 0.f);
    ground->scale = Eigen::Vector3f(10.f, 10.f, 10.f);
    ground->rotation.x() = -90;

    prog = std::shared_ptr<pxl::Program>(new pxl::Program(
        GetShaderPath("mesh.vert"), GetShaderPath("mesh.frag")));

    camera = std::make_shared<pxl::Camera>();
    camera->position += Eigen::Vector3f(0, 1, 10);
    camera->AddComponent(std::make_shared<pxl::FreeCameraComponent>());

    framebuffers =
        std::make_pair(std::make_shared<pxl::OglFramebuffer>(1920, 1080),
                       std::make_shared<pxl::OglFramebuffer>(1920, 1080));
    framebuffers.first->Bind();
    framebuffers.second->Bind();

    fxaa_output = std::make_shared<pxl::OglTexture2d>(
        1920, 1080, pxl::Texture2d::Format::FLOAT);
    fxaa_output->Bind();

    point_lights.emplace_back(new pxl::PointLight(pxl::Color(1.f, .0f, .0f)));
    point_lights.back()->position += Eigen::Vector3f(4.f, 2.f, 0.f);
    point_lights.emplace_back(new pxl::PointLight(pxl::Color(.0f, .0f, 1.f)));
    point_lights.back()->position += Eigen::Vector3f(-4.f, 2.f, 0.f);
    point_lights.emplace_back(new pxl::PointLight(pxl::Color(.0f, 1.f, .0f)));
    point_lights.back()->position += Eigen::Vector3f(-8.f, 2.f, 0.f);
    point_lights.emplace_back(new pxl::PointLight(pxl::Color(.5f, .0f, 1.f)));
    point_lights.back()->position += Eigen::Vector3f(8.f, 2.f, 0.f);

    auto light = std::make_shared<pxl::PointLight>(pxl::Color(1.f, 1.f, 1.f));
    light->position += Eigen::Vector3f(1.f, 1.f, 1.f);
    child->AddChild(light);

    auto light2 = std::make_shared<pxl::PointLight>(pxl::Color(1.f, .5f, 0.f));
    light2->position += Eigen::Vector3f(1.f, 1.f, 1.f);
    light->AddChild(light2);

    scene = std::make_shared<pxl::Scene>();
    scene->camera = camera;
    scene->AddEntity(camera);
    scene->AddEntity(mesh);
    scene->AddEntity(child);
    // scene->AddEntity(child_child);
    scene->AddEntity(ground);
    scene->AddEntity(light);
    scene->AddEntity(light2);
    // scene->AddEntity(
    //    std::make_shared<pxl::PointLight>(pxl::Color(1.f, 1.f, 1.f)));
    // scene->entities.back()->position += Eigen::Vector3f(0.f, 3.f, 0.f);
    scene->AddEntities(point_lights);
                           
    scene->Bind();
  }

  void Update(float time_elapsed) override {
    static float gamma = 2.2f;
    scene->Update(time_elapsed);
    framebuffers.first->Begin();
    // prog->Bind();

    mesh->rotation += Eigen::Vector3f(0, 90 * time_elapsed, 0);
    mesh->rotation.y() = fmod(mesh->rotation.y(), 360.f);
    if (ImGui::Begin("Property")) {
      ImGui::DragFloat3("Position", mesh->position.data());
      ImGui::DragFloat3("Rotation", mesh->rotation.data());
      ImGui::DragFloat3("Scale", mesh->scale.data());
      ImGui::DragFloat("Gamma", &gamma, .01f);
      ImGui::End();
    }

    pxl::SceneRenderer::RenderScene(*scene, framebuffers.first);
    framebuffers.first->End();

    framebuffers.second->Begin();
    pxl::OglGammaRenderer::GetInstance()->RenderTexture(
        framebuffers.first->GetColorAttachment(0), gamma);
    framebuffers.second->End();

    pxl::OglFxaaRenderer::GetInstance()->RenderTexture(
        *framebuffers.second->GetColorAttachment(0));
  }

  std::pair<std::shared_ptr<pxl::OglFramebuffer>,
            std::shared_ptr<pxl::OglFramebuffer>>
      framebuffers;
  std::shared_ptr<pxl::OglTexture2d> fxaa_output;
  std::shared_ptr<pxl::Camera> camera;
  std::shared_ptr<pxl::Scene> scene;
  std::shared_ptr<pxl::MeshEntity> mesh;
  std::shared_ptr<pxl::MeshEntity> ground;
  std::shared_ptr<pxl::Program> prog;
  std::vector<std::shared_ptr<pxl::Entity>> point_lights;
};

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);
  HelloGame game;
  game.Run();
  return 0;
}
