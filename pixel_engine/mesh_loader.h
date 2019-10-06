#pragma once

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glog/logging.h>
#include <assimp/Importer.hpp>
#include <boost/filesystem.hpp>

#include <pixel_engine/mesh.h>

// Removes any leftover lines or points from the mesh
#ifdef AI_CONFIG_PP_SBP_REMOVE
#undef AI_CONFIG_PP_SBP_REMOVE
#endif  // AI_CONFIG_PP_SBP_REMOVE
#define AI_CONFIG_PP_SBP_REMOVE (aiPrimitiveType_POINTS | aiPrimitiveType_LINES)

namespace pxl {
class MeshLoader {
  static std::shared_ptr<SubMesh> ParseMesh(const aiMesh *ai_mesh);

  static std::shared_ptr<Material> ParseMaterial(const aiMaterial *ai_material);

  template <typename MeshType>
  static std::shared_ptr<MeshType> ParseScene(const aiScene *ai_scene) {
    MeshType mesh;
    for (size_t i = 0; i < ai_scene->mNumMeshes; ++i) {
      const aiMesh *ai_mesh = ai_scene->mMeshes[i];
      mesh.sub_meshes.emplace_back(std::move(ParseMesh(ai_mesh)));
    }

    for (size_t i = 0; i < ai_scene->mNumMaterials; ++i) {
      const aiMaterial *ai_material = ai_scene->mMaterials[i];
      mesh.materials.emplace_back(std::move(ParseMaterial(ai_material)));
    }
    return std::make_shared<MeshType>(mesh);
  }

  template <typename MeshType>
  static void FixMaterialPaths(std::shared_ptr<MeshType> mesh,
                               const boost::filesystem::path &path) {
    for (auto material : mesh->materials) {
      if (!material->diffuse_texture_path.empty()) {
        material->diffuse_texture_path =
            path.parent_path() / material->diffuse_texture_path;
      }
    }
  }

 public:
  template <typename MeshType>
  static std::shared_ptr<MeshType> LoadMesh(
      const boost::filesystem::path &path) {
    Assimp::Importer importer;

    uint32_t read_flags = aiProcess_JoinIdenticalVertices |
                          aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                          aiProcess_RemoveRedundantMaterials |
                          aiProcess_FindDegenerates | aiProcess_SortByPType |
                          aiProcess_GenUVCoords | aiProcess_FindInvalidData |
                          aiProcess_FindInstances | aiProcess_OptimizeMeshes |
                          aiProcess_OptimizeGraph;

    const aiScene *ai_scene = importer.ReadFile(path.string(), read_flags);
    if (ai_scene == nullptr) {
      LOG(ERROR) << "Unable to load mesh " << path;
      return nullptr;
    }

    std::shared_ptr<MeshType> mesh = ParseScene<MeshType>(ai_scene);
    FixMaterialPaths(mesh, path);
    return std::move(mesh);
  }
};  // namespace pxl
}  // namespace pxl
