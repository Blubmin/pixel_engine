#include <pixel_engine/convex_hull.h>

#include <cfloat>
#include <stack>
#include <unordered_set>
#include <vector>

#include <glog/logging.h>
#include <Eigen/Geometry>
#include <boost/functional/hash.hpp>

#include <pixel_engine/eigen_utilities.h>

namespace {
struct Edge {
  Eigen::Vector3f start, end;

  Edge() {}

  Edge(const Eigen::Vector3f& start, const Eigen::Vector3f& end)
      : start(start), end(end) {}

  bool operator==(const Edge& e) const {
    return (start.isApprox(e.start, FLT_EPSILON) &&
            end.isApprox(e.end, FLT_EPSILON)) ||
           (start.isApprox(e.end, FLT_EPSILON) &&
            end.isApprox(e.start, FLT_EPSILON));
  }

  friend std::ostream& operator<<(std::ostream& os, const Edge& e) {
    return (os << "(" << e.start.transpose() << ")->(" << e.end.transpose())
           << ")";
  }
};

struct Face {
  Edge e1, e2, e3;
  Eigen::Vector3f center;
  Eigen::Vector3f normal;
  std::unordered_set<Eigen::Vector3f> verts;
  Eigen::Vector3f max_vert;

  Face(const Edge& e1, const Edge& e2, const Edge& e3,
       std::unordered_set<Eigen::Vector3f>& working_verticies)
      : e1(e1), e2(e2), e3(e3) {
    center = (e1.start + e2.start + e3.start) / 3.f;
    normal = (e2.start - e1.start).cross(e3.start - e1.start);

    working_verticies.erase(e1.start);
    working_verticies.erase(e2.start);
    working_verticies.erase(e3.start);

    for (auto v : working_verticies) {
      if (CanSee(v)) {
        verts.insert(v);
      }
    }

    for (auto v : verts) {
      working_verticies.erase(v);
    }

    float max_dist = -1;
    for (auto v : verts) {
      float dist = std::abs(normal.dot(v - center));
      if (dist > max_dist) {
        max_vert = v;
        max_dist = dist;
      }
    }
  }

  bool operator==(const Face& f) {
    return e1 == f.e1 && e2 == f.e2 && e3 == f.e3;
  }

  bool OnHull() const { return verts.empty(); }

  bool CanSee(const Eigen::Vector3f& v) const {
    return normal.dot(v - center) > 0;
  }
};

Face CreateFace(const Eigen::Vector3f& p1, const Eigen::Vector3f& p2,
                const Eigen::Vector3f& p3, const Eigen::Vector3f& center,
                std::unordered_set<Eigen::Vector3f>& working_verticies) {
  Eigen::Vector3f face_center = (p1 + p2 + p3) / 3.f;
  Eigen::Vector3f center_ray = face_center - center;
  Eigen::Vector3f face_normal = (p2 - p1).cross(p3 - p1);

  Edge e1, e2, e3;
  if (face_normal.dot(center_ray) > 0) {
    e1 = Edge(p1, p2);
    e2 = Edge(p2, p3);
    e3 = Edge(p3, p1);
  } else {
    e1 = Edge(p1, p3);
    e2 = Edge(p3, p2);
    e3 = Edge(p2, p1);
  }

  return Face(e1, e2, e3, working_verticies);
}
}  // namespace

namespace std {
template <>
struct hash<Edge> {
  size_t operator()(const Edge& edge) const {
    size_t seed = 0;
    seed += std::hash<Eigen::Vector3f>()(edge.start);
    seed += std::hash<Eigen::Vector3f>()(edge.end);
    return seed;
  }
};
}  // namespace std

namespace pxl {
ConvexHull::ConvexHull() {}

ConvexHull::ConvexHull(const Mesh* mesh) {
  sub_meshes.resize(mesh->sub_meshes.size());
#pragma omp parallel for
  for (int64_t i = 0; i < mesh->sub_meshes.size(); ++i) {
    sub_meshes[i] = QuickHull(*mesh->sub_meshes[i]);
  }

  materials.resize(mesh->materials.size());
  std::copy(mesh->materials.begin(), mesh->materials.end(), materials.begin());
}

// Runs the quick hull algorithm to compute the convex hull of the given sub
// mesh
std::shared_ptr<SubMesh> ConvexHull::QuickHull(const SubMesh& mesh) {
  std::shared_ptr<SubMesh> sub_mesh = std::make_shared<SubMesh>();
  LOG(INFO) << "Start";

  // Convert loose floats into vertices
  std::unordered_set<Eigen::Vector3f> verticies;
  Eigen::Vector3f mean = Eigen::Vector3f::Zero();
  for (uint32_t i = 0; i < mesh.positions.size(); i += 3) {
    Eigen::Vector3f new_vert(mesh.positions[i], mesh.positions[i + 1],
                             mesh.positions[i + 2]);
    verticies.emplace(new_vert);
    mean += new_vert;
  }
  mean /= verticies.size();

  // Finds the furthest apart verticies
  Eigen::Vector3f extreme1;
  float max_dist = -1;
  for (auto i = verticies.begin(); i != verticies.end(); ++i) {
    float dist = (*i - mean).squaredNorm();
    if (dist > max_dist) {
      extreme1 = *i;
      max_dist = dist;
    }
  }
  Eigen::Vector3f extreme2;
  max_dist = -1;
  for (auto j = verticies.begin(); j != verticies.end(); ++j) {
    float dist = (extreme1 - *j).squaredNorm();
    if (dist > max_dist) {
      extreme2 = *j;
      max_dist = dist;
    }
  }

  // Finds the largest triangle
  Eigen::Vector3f extreme3;
  float max_area = -1;
  Eigen::Vector3f edge(extreme2 - extreme1);
  for (auto k = verticies.begin(); k != verticies.end(); ++k) {
    float area = edge.cross(*k - extreme1).squaredNorm();
    if (area > max_area) {
      extreme3 = *k;
      max_area = area;
    }
  }

  // Gets the normal of that triangle
  Eigen::Vector3f normal =
      (extreme3 - extreme1).cross(extreme3 - extreme2).normalized();

  // Finds a fourth point that creates the largest tetrahedron
  Eigen::Vector3f extreme4;
  float max_volume = -1;
  for (auto l = verticies.begin(); l != verticies.end(); ++l) {
    float volume = std::abs(normal.dot(*l - extreme1));
    if (volume > max_volume) {
      extreme4 = *l;
      max_volume = volume;
    }
  }

  // Used to contruct the correct face directions
  Eigen::Vector3f center = (extreme1 + extreme2 + extreme3 + extreme4) / 4.f;

  Face f1 = CreateFace(extreme1, extreme2, extreme3, center, verticies);
  Face f2 = CreateFace(extreme2, extreme3, extreme4, center, verticies);
  Face f3 = CreateFace(extreme1, extreme2, extreme4, center, verticies);
  Face f4 = CreateFace(extreme1, extreme3, extreme4, center, verticies);

  std::vector<Face> faces{f1, f2, f3, f4};
  std::stack<Face> worklist;
  worklist.push(f1);
  worklist.push(f2);
  worklist.push(f3);
  worklist.push(f4);

  LOG(INFO) << "Init";

  // Iterate through each face, expanding the final result as you go
  while (!worklist.empty()) {
    Face f = worklist.top();
    worklist.pop();

    // If the face has no verts outside the hull, then continue
    if (f.OnHull()) {
      continue;
    }

    // Determine if any of the faces can see the max point of this face and mark
    // them for removal (i.e. they are on the interior of the convex hull)
    std::vector<Face> to_remove;
    std::unordered_set<Edge> can_see, not_see;
    for (const auto& face : faces) {
      if (face.CanSee(f.max_vert)) {
        can_see.insert(face.e1);
        can_see.insert(face.e2);
        can_see.insert(face.e3);
        to_remove.push_back(face);
      } else {
        not_see.insert(face.e1);
        not_see.insert(face.e2);
        not_see.insert(face.e3);
      }
    }

    // Figure out which edges are on the "horizon" of the max vert from the
    // worklist face
    std::vector<Edge> horizon;
    for (const auto& e : can_see) {
      auto begin = not_see.begin();
      if (not_see.find(e) != not_see.end()) {
        horizon.push_back(e);
      }
    }

    // Construct faces connecting the horizon edges and the max vert of the
    // worklist face
    for (const auto& h : horizon) {
      Face new_face = CreateFace(h.start, h.end, f.max_vert, center, f.verts);
      worklist.push(new_face);
      faces.push_back(new_face);
    }

    // Remove any interior faces
    for (const auto& face : to_remove) {
      faces.erase(std::remove(faces.begin(), faces.end(), face), faces.end());
    }
  }

  // Convert face format for rendering
  for (const Face& f : faces) {
    sub_mesh->positions.push_back(f.e1.start.x());
    sub_mesh->positions.push_back(f.e1.start.y());
    sub_mesh->positions.push_back(f.e1.start.z());
    sub_mesh->positions.push_back(f.e2.start.x());
    sub_mesh->positions.push_back(f.e2.start.y());
    sub_mesh->positions.push_back(f.e2.start.z());
    sub_mesh->positions.push_back(f.e3.start.x());
    sub_mesh->positions.push_back(f.e3.start.y());
    sub_mesh->positions.push_back(f.e3.start.z());

    Eigen::Vector3f normal = f.normal.normalized();
    sub_mesh->normals.push_back(normal.x());
    sub_mesh->normals.push_back(normal.y());
    sub_mesh->normals.push_back(normal.z());
    sub_mesh->normals.push_back(normal.x());
    sub_mesh->normals.push_back(normal.y());
    sub_mesh->normals.push_back(normal.z());
    sub_mesh->normals.push_back(normal.x());
    sub_mesh->normals.push_back(normal.y());
    sub_mesh->normals.push_back(normal.z());

    Eigen::Vector3f tangent = normal.cross(Eigen::Vector3f::UnitY());
    sub_mesh->tangents.push_back(tangent.x());
    sub_mesh->tangents.push_back(tangent.y());
    sub_mesh->tangents.push_back(tangent.z());
    sub_mesh->tangents.push_back(tangent.x());
    sub_mesh->tangents.push_back(tangent.y());
    sub_mesh->tangents.push_back(tangent.z());
    sub_mesh->tangents.push_back(tangent.x());
    sub_mesh->tangents.push_back(tangent.y());
    sub_mesh->tangents.push_back(tangent.z());

    sub_mesh->texture_coordinates.push_back(0);
    sub_mesh->texture_coordinates.push_back(0);
    sub_mesh->texture_coordinates.push_back(0);
    sub_mesh->texture_coordinates.push_back(0);
    sub_mesh->texture_coordinates.push_back(0);
    sub_mesh->texture_coordinates.push_back(0);

    sub_mesh->triangles.push_back(sub_mesh->triangles.size());
    sub_mesh->triangles.push_back(sub_mesh->triangles.size());
    sub_mesh->triangles.push_back(sub_mesh->triangles.size());
  }
  LOG(INFO) << "Done";
  sub_mesh->material_idx = mesh.material_idx;
  return sub_mesh;
}
}  // namespace pxl