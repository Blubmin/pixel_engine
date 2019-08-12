#pragma once

#include <GL/gl3w.h>>

#include <pixel_engine/camera.h>
#include <pixel_engine/entity.h>

namespace pxl {
class Scene {
 public:
  void Update(float time_elapsed);

  void Bind();
  void DrawGrid() const;
  void DrawPose() const;

  std::vector<std::shared_ptr<Entity>> entities;
  std::shared_ptr<Camera> camera;

  template <typename EntityType>
  std::vector<std::shared_ptr<EntityType>> GetEntities() const {
    std::vector<std::shared_ptr<EntityType>> entities_by_type;
    for (auto entity : entities) {
      std::shared_ptr<EntityType> typed_entity =
          std::dynamic_pointer_cast<EntityType>(entity);
      if (typed_entity != nullptr) {
        entities_by_type.push_back(typed_entity);
      }
    }
    return entities_by_type;
  }

 private:
  GLuint grid_vao_;
  GLuint grid_buffer_;
  GLuint pose_vao_;
  GLuint pose_buffers_[2];
};
}  // namespace pxl
