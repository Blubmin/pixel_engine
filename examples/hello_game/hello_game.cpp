#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/gl3w.h>>
#include <glog/logging.h>
#include <pixel_engine/game.h>
#include <pixel_engine/mesh_loader.h>
#include <pixel_engine/ogl_mesh.h>
#include <pixel_engine/program.h>
#include <Eigen/Geometry>

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
    mesh = pxl::MeshLoader::LoadMesh<pxl::OglMesh>(GetMeshPath("plane.obj"));
    mesh->Bind();
    prog = std::shared_ptr<pxl::Program>(new pxl::Program(
        GetShaderPath("mesh.vert"), GetShaderPath("mesh.frag")));
  }
  void Loop() override {
    prog->Bind();
    Eigen::Isometry3f u_model = Eigen::Isometry3f::Identity();
    u_model.translate(Eigen::Vector3f::UnitZ() * -5);
    u_model.translate(Eigen::Vector3f::UnitY());

    Eigen::Isometry3f u_view = Eigen::Isometry3f::Identity();

    Eigen::Matrix4f u_perspective = Eigen::Matrix4f::Zero();
    float near_plane = .01f;
    float far_plane = 1000.f;
    float t =
        1 / (std::tan(90.f / 2.f * M_PI / 180.f) * State.GetAspectRatio());
    float s = 1 / (std::tan(90.f / 2.f * M_PI / 180.f));
    u_perspective(0, 0) = t;
    u_perspective(1, 1) = s;
    u_perspective(2, 2) = (far_plane + near_plane) / (near_plane - far_plane);
    u_perspective(2, 3) =
        (2 * far_plane * near_plane) / (near_plane - far_plane);
    u_perspective(3, 2) = -1;

    glUniformMatrix4fv(prog->GetUniformLocation("u_model"), 1, GL_FALSE,
                       u_model.matrix().data());
    glUniformMatrix4fv(prog->GetUniformLocation("u_view"), 1, GL_FALSE,
                       u_view.matrix().data());
    glUniformMatrix4fv(prog->GetUniformLocation("u_perspective"), 1, GL_FALSE,
                       u_perspective.data());

    mesh->Draw();
  }

  std::shared_ptr<pxl::OglMesh> mesh;
  std::shared_ptr<pxl::Program> prog;
};

int main(int argc, char* argv[]) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);
  HelloGame game;
  game.Run();
  return 0;
}
