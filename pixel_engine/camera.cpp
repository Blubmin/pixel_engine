#include <pixel_engine/camera.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <Eigen/Geometry>

#include <pixel_engine/game.h>
#include <pixel_engine/mesh_loader.h>
#include <pixel_engine/ogl_mesh.h>
#include <pixel_engine/ogl_utilities.h>

namespace pxl {
std::shared_ptr<MeshEntity> Camera::mesh(nullptr);

Camera::Camera()
    : fov(90.f), near_plane(0.01f), far_plane(1000.f), draw_frustum_(false) {}

void Camera::Draw(const Program& prog) {
  if (mesh == nullptr) {
    mesh = MeshLoader::LoadMeshEntity<OglMesh>(
        (boost::filesystem::path(__FILE__).parent_path() / "meshes" / "camera" /
         "camera.obj"));
    CHECK(mesh != nullptr);
    mesh->Bind();
  }
  mesh->Draw(prog);

  if (draw_frustum_) {
    std::vector<float> pts;
    std::vector<GLuint> idx;

    auto frustum_pts = GetFrustumPoints();

    pts.push_back(frustum_pts[0].x());
    pts.push_back(frustum_pts[0].y());
    pts.push_back(frustum_pts[0].z());
    pts.push_back(frustum_pts[1].x());
    pts.push_back(frustum_pts[1].y());
    pts.push_back(frustum_pts[1].z());
    pts.push_back(frustum_pts[2].x());
    pts.push_back(frustum_pts[2].y());
    pts.push_back(frustum_pts[2].z());
    pts.push_back(frustum_pts[3].x());
    pts.push_back(frustum_pts[3].y());
    pts.push_back(frustum_pts[3].z());
    pts.push_back(frustum_pts[4].x());
    pts.push_back(frustum_pts[4].y());
    pts.push_back(frustum_pts[4].z());
    pts.push_back(frustum_pts[5].x());
    pts.push_back(frustum_pts[5].y());
    pts.push_back(frustum_pts[5].z());
    pts.push_back(frustum_pts[6].x());
    pts.push_back(frustum_pts[6].y());
    pts.push_back(frustum_pts[6].z());
    pts.push_back(frustum_pts[7].x());
    pts.push_back(frustum_pts[7].y());
    pts.push_back(frustum_pts[7].z());

    idx.push_back(0);
    idx.push_back(1);
    idx.push_back(0);
    idx.push_back(2);
    idx.push_back(1);
    idx.push_back(3);
    idx.push_back(2);
    idx.push_back(3);
    idx.push_back(0);
    idx.push_back(4);
    idx.push_back(1);
    idx.push_back(5);
    idx.push_back(2);
    idx.push_back(6);
    idx.push_back(3);
    idx.push_back(7);
    idx.push_back(4);
    idx.push_back(5);
    idx.push_back(4);
    idx.push_back(6);
    idx.push_back(5);
    idx.push_back(7);
    idx.push_back(6);
    idx.push_back(7);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffers[2];
    glGenBuffers(2, buffers);

    OGL_LOG_ERROR(INFO);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(GLfloat), pts.data(),
                 GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    OGL_LOG_ERROR(INFO);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
    OGL_LOG_ERROR(INFO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint),
                 idx.data(), GL_STATIC_DRAW);
    OGL_LOG_ERROR(INFO);

    glDrawElements(GL_LINES, idx.size(), GL_UNSIGNED_INT, 0);
    OGL_LOG_ERROR(INFO);
    glDeleteBuffers(2, buffers);
    OGL_LOG_ERROR(INFO);
    glDeleteVertexArrays(1, &vao);
  }
}

std::vector<Eigen::Vector3f> Camera::GetFrustumPoints() const {
  float half_tan = std::tan(fov / 2.f * M_PI / 180.f);
  Eigen::Vector3f near_vec = -Eigen::Vector3f::UnitZ() * near_plane;
  Eigen::Vector3f near_right_vec =
      Eigen::Vector3f::UnitX() * near_plane * half_tan;
  Eigen::Vector3f near_up_vec = Eigen::Vector3f::UnitY() * near_plane *
                                half_tan / Game::State.GetAspectRatio();

  Eigen::Vector3f far_vec = -Eigen::Vector3f::UnitZ() * far_plane;
  Eigen::Vector3f far_right_vec =
      Eigen::Vector3f::UnitX() * far_plane * half_tan;
  Eigen::Vector3f far_up_vec = Eigen::Vector3f::UnitY() * far_plane * half_tan /
                               Game::State.GetAspectRatio();

  std::vector<Eigen::Vector3f> pts(8);
  pts[0] = near_vec - near_right_vec + near_up_vec;
  pts[1] = near_vec + near_right_vec + near_up_vec;
  pts[2] = near_vec - near_right_vec - near_up_vec;
  pts[3] = near_vec + near_right_vec - near_up_vec;
  pts[4] = far_vec - far_right_vec + far_up_vec;
  pts[5] = far_vec + far_right_vec + far_up_vec;
  pts[6] = far_vec - far_right_vec - far_up_vec;
  pts[7] = far_vec + far_right_vec - far_up_vec;
  return pts;
}

Eigen::Matrix4f Camera::GetPerspective() const {
  Eigen::Matrix4f perspective = Eigen::Matrix4f::Zero();
  float half_tan = std::tan(fov / 2.f * M_PI / 180.f);
  perspective(0, 0) = 1.f / half_tan;
  perspective(1, 1) = 1.f / (half_tan / Game::State.GetAspectRatio());
  perspective(2, 2) = (far_plane + near_plane) / (near_plane - far_plane);
  perspective(2, 3) = (2.f * far_plane * near_plane) / (near_plane - far_plane);
  perspective(3, 2) = -1;
  return perspective;
}

Eigen::Matrix4f Camera::GetView() const {
  Eigen::Affine3f transform(GetTransform());
  Eigen::Isometry3f isometry;
  isometry.translation() = transform.translation();
  isometry.linear() = transform.rotation();
  return isometry.inverse().matrix();
}
}  // namespace pxl
