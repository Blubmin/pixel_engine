#include <pixel_engine/mesh_loader.h>

#include <string>

namespace pxl {
std::shared_ptr<SubMesh> MeshLoader::ParseMesh(const aiMesh* ai_mesh) {
  SubMesh sub_mesh;
  if (ai_mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
    return nullptr;
  }
  // Triangles
  for (size_t i = 0; i < ai_mesh->mNumFaces; ++i) {
    const aiFace& face = ai_mesh->mFaces[i];
    sub_mesh.triangles.push_back(face.mIndices[0]);
    sub_mesh.triangles.push_back(face.mIndices[1]);
    sub_mesh.triangles.push_back(face.mIndices[2]);
  }
  for (size_t i = 0; i < ai_mesh->mNumVertices; ++i) {
    // Positions
    const aiVector3D& vert = ai_mesh->mVertices[i];
    sub_mesh.positions.push_back(vert.x);
    sub_mesh.positions.push_back(vert.y);
    sub_mesh.positions.push_back(vert.z);

    // Normals
    const aiVector3D& normal = ai_mesh->mNormals[i];
    sub_mesh.normals.push_back(normal.x);
    sub_mesh.normals.push_back(normal.y);
    sub_mesh.normals.push_back(normal.z);

    // UV Mapping
    if (ai_mesh->HasTextureCoords(0)) {
      const aiVector3D& texture_coord = ai_mesh->mTextureCoords[0][i];
      sub_mesh.texture_coordinates.push_back(texture_coord.x);
      sub_mesh.texture_coordinates.push_back(texture_coord.y);
    } else {
      sub_mesh.texture_coordinates.push_back(0);
      sub_mesh.texture_coordinates.push_back(0);
    }
  }

  sub_mesh.material_idx = ai_mesh->mMaterialIndex;
  return std::make_shared<SubMesh>(std::move(sub_mesh));
}

std::shared_ptr<Material> MeshLoader::ParseMaterial(
    const aiMaterial* ai_material) {
  Material material;

  // Ambeint
  aiColor3D ambient;
  if (AI_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_AMBIENT, ambient)) {
    ambient = aiColor3D(0, 0, 0);
  }
  material.ambient = Eigen::Vector3f(ambient.r, ambient.g, ambient.b);

  // Diffuse
  aiColor3D diffuse;
  if (AI_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse)) {
    diffuse = aiColor3D(0, 0, 0);
  }
  material.diffuse = Eigen::Vector3f(diffuse.r, diffuse.g, diffuse.b);

  // Specular
  aiColor3D specular;
  if (AI_SUCCESS != ai_material->Get(AI_MATKEY_COLOR_SPECULAR, specular)) {
    specular = aiColor3D(0, 0, 0);
  }
  material.specular = Eigen::Vector3f(specular.r, specular.g, specular.b);

  // Shininess
  float shininess;
  if (AI_SUCCESS != ai_material->Get(AI_MATKEY_SHININESS, shininess)) {
    shininess = 0;
  }
  material.shininess = shininess;

  // Diffuse Texture
  aiString diffuse_texture_path;
  if (AI_SUCCESS == ai_material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0),
                                     diffuse_texture_path) &&
      diffuse_texture_path.length != 0) {
    material.diffuse_texture_path =
        boost::filesystem::path(diffuse_texture_path.data);
  }

  return std::move(std::make_shared<Material>(material));
}
}  // namespace pxl
