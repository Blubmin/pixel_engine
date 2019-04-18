#include <pixel_engine/mesh_loader.h>

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
    }
  }
  return std::make_shared<SubMesh>(std::move(sub_mesh));
}
}  // namespace pxl
