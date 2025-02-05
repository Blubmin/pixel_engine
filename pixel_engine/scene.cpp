#include <pixel_engine/scene.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <btBulletCollisionCommon.h>
#include <glog/logging.h>

#include <pixel_engine/collider_component.h>
#include <pixel_engine/physics_component.h>

namespace pxl {
namespace {
const int kGridSize = 10;
const float kAcceleration = -9.8;
}  // namespace
Scene::Scene() : debug_draw(false) {}

void Scene::Update(float time_elapsed) {
  // Add new entities to the scene
  for (auto add : added_entities_) {
    entities_.push_back(add);
  }
  added_entities_.clear();

  // Remove entities from the scene
  entities_.erase(std::remove_if(entities_.begin(), entities_.end(), [&](auto& entity) {
    return std::find(removed_entities_.begin(), removed_entities_.end(),
                     entity) != removed_entities_.end();
  }), entities_.end());
  removed_entities_.clear();

  // Update all entities in the scene
  for (auto entity : entities_) {
    entity->Update(time_elapsed);
    auto physics = entity->GetComponent<PhysicsComponent>();
    if (physics == nullptr) {
      continue;
    }
    physics->velocity +=
        Eigen::Vector3f::UnitY() * kAcceleration * time_elapsed;
    entity->position += physics->velocity * time_elapsed;
    if (entity->position.y() < 0) {
      entity->position.y() = 0;
      // physics->velocity = Eigen::Vector3f::Zero();
      physics->velocity.y() = 0;
    }
  }

  // Bullet collision handling
  auto config = new btDefaultCollisionConfiguration();
  auto dispatcher = new btCollisionDispatcher(config);
  btVector3 world_min(-500, -500, -500);
  btVector3 world_max(500, 500, 500);
  auto broad =
      new bt32BitAxisSweep3(world_min, world_max, entities_.size(), 0, true);
  auto world = new btCollisionWorld(dispatcher, broad, config);

  for (auto entity : entities_) {
    auto collider = entity->GetComponent<ColliderComponent>();
    if (collider == nullptr) {
      continue;
    }
    auto object = collider->GetCollisionObject();
    world->addCollisionObject(object);
  }
  world->performDiscreteCollisionDetection();

  // Collision responses
  auto num_manifolds = world->getDispatcher()->getNumManifolds();
  for (int i = 0; i < num_manifolds; ++i) {
    auto manifold = world->getDispatcher()->getManifoldByIndexInternal(i);
    if (manifold->getNumContacts() != 1) {
      continue;
    }
    auto pt = manifold->getContactPoint(0);
    auto object1 = manifold->getBody0();
    auto collider1 = (ColliderComponent*)object1->getUserPointer();
    auto type1 = collider1->GetType();
    auto object2 = manifold->getBody1();
    auto collider2 = (ColliderComponent*)object2->getUserPointer();
    auto type2 = collider2->GetType();


    // Call custom collision responses
    auto responses = collider1->GetOwner()->GetComponents<CollisionResponse>();
    for (auto response : responses) {
      response->Respond(pt, object1, object2);
    }
    responses = collider2->GetOwner()->GetComponents<CollisionResponse>();
    for (auto response : responses) {
      response->Respond(pt, object2, object1);
    }


    auto translation_vec = pt.m_normalWorldOnB.normalized() * pt.getDistance();
    auto angle = std::abs(btVector3(0, pt.m_normalWorldOnB.y() > 0 ? 1 : -1, 0)
                              .dot(pt.m_normalWorldOnB));
    auto limit = std::cos(46 * M_PI / 180.f);  // Set slope limit to 46 degrees
    if (angle < limit) {
      translation_vec.setY(0);

      if (type1 == ColliderComponent::kStatic &&
          type2 == ColliderComponent::kDynamic) {
        collider2->owner.lock()->position += Eigen::Vector3f(
            translation_vec.x(), translation_vec.y(), translation_vec.z());

      } else if (type1 == ColliderComponent::kDynamic &&
                 type2 == ColliderComponent::kStatic) {
        collider1->owner.lock()->position -= Eigen::Vector3f(
            translation_vec.x(), translation_vec.y(), translation_vec.z());
      } else if (type1 == ColliderComponent::kDynamic &&
                 type2 == ColliderComponent::kDynamic) {
        collider1->owner.lock()->position -=
            (Eigen::Vector3f(translation_vec.x(), translation_vec.y(),
                             translation_vec.z()) /
             2);
        collider2->owner.lock()->position +=
            (Eigen::Vector3f(translation_vec.x(), translation_vec.y(),
                             translation_vec.z()) /
             2);
      }
    } else {
      if (type1 == ColliderComponent::kStatic &&
          type2 == ColliderComponent::kDynamic) {
        auto from = object2->getWorldTransform().getOrigin();
        auto to =
            object2->getWorldTransform().getOrigin() - btVector3(0, 100, 0);
        auto cb = btCollisionWorld::AllHitsRayResultCallback(from, to);
        world->rayTest(from, to, cb);
        for (int i = 0; i < cb.m_collisionObjects.size(); ++i) {
          const auto obj = cb.m_collisionObjects[i];
          if (obj == object2) {
            continue;
          }
          auto hit_pt = from + (to - from) * cb.m_hitFractions[i];
          collider2->owner.lock()->position =
              Eigen::Vector3f(hit_pt.x(), hit_pt.y(), hit_pt.z());
          break;
        }
      } else if (type1 == ColliderComponent::kDynamic &&
                 type2 == ColliderComponent::kStatic) {
        auto from = object1->getWorldTransform().getOrigin();
        auto to =
            object1->getWorldTransform().getOrigin() - btVector3(0, 100, 0);
        auto cb = btCollisionWorld::AllHitsRayResultCallback(from, to);
        world->rayTest(from, to, cb);
        for (int i = 0; i < cb.m_collisionObjects.size(); ++i) {
          const auto obj = cb.m_collisionObjects[i];
          if (obj == object1) {
            continue;
          }
          auto hit_pt = from + (to - from) * cb.m_hitFractions[i];
          collider1->owner.lock()->position =
              Eigen::Vector3f(hit_pt.x(), hit_pt.y(), hit_pt.z());
          break;
        }
      } else if (type1 == ColliderComponent::kDynamic &&
                 type2 == ColliderComponent::kDynamic) {
        collider1->owner.lock()->position -=
            (Eigen::Vector3f(translation_vec.x(), translation_vec.y(),
                             translation_vec.z()) /
             2);
        collider2->owner.lock()->position +=
            (Eigen::Vector3f(translation_vec.x(), translation_vec.y(),
                             translation_vec.z()) /
             2);
      }
    }

    auto physics = collider1->owner.lock()->GetComponent<PhysicsComponent>();
    if (physics != nullptr &&
        (btVector3(0, 1, 0).dot(pt.m_normalWorldOnB) > .707 ||
         btVector3(0, 1, 0).dot(-pt.m_normalWorldOnB) > .707)) {
      physics->velocity.y() = 0;
    }
    if (physics != nullptr && type1 == ColliderComponent::kDynamic) {
      Eigen::Vector3f norm =
          Eigen::Map<Eigen::Vector3f>(pt.m_normalWorldOnB.m_floats);
      physics->velocity = physics->velocity - 2 * (physics->velocity.dot(norm)) * norm;
      physics->velocity *= .5;
    }


    physics = collider2->owner.lock()->GetComponent<PhysicsComponent>();
    if (physics != nullptr &&
        (btVector3(0, 1, 0).dot(-pt.m_normalWorldOnB) > .707 ||
         btVector3(0, 1, 0).dot(-pt.m_normalWorldOnB) > .707)) {
      physics->velocity.y() = 0;
    }
    if (physics != nullptr && type2 == ColliderComponent::kDynamic) {
      Eigen::Vector3f norm = -Eigen::Map<Eigen::Vector3f>(pt.m_normalWorldOnB.m_floats);
      physics->velocity =
          physics->velocity - 2 * (physics->velocity.dot(norm)) * norm;
      physics->velocity *= .5;
    }
  }

  delete world;
  delete broad;
  delete dispatcher;
  delete config;
}

void Scene::Bind() {
  // Grid
  std::vector<Eigen::Vector2f> grid_positions;
  grid_positions.emplace_back(0, kGridSize);
  grid_positions.emplace_back(0, -kGridSize);
  grid_positions.emplace_back(kGridSize, 0);
  grid_positions.emplace_back(-kGridSize, 0);
  for (int i = 1; i <= kGridSize; ++i) {
    grid_positions.emplace_back(i, kGridSize);
    grid_positions.emplace_back(i, -kGridSize);
    grid_positions.emplace_back(-i, kGridSize);
    grid_positions.emplace_back(-i, -kGridSize);
    grid_positions.emplace_back(kGridSize, i);
    grid_positions.emplace_back(-kGridSize, i);
    grid_positions.emplace_back(kGridSize, -i);
    grid_positions.emplace_back(-kGridSize, -i);
  }
  glGenVertexArrays(1, &grid_vao_);
  glBindVertexArray(grid_vao_);
  glGenBuffers(1, &grid_buffer_);

  // Positions
  glBindBuffer(GL_ARRAY_BUFFER, grid_buffer_);
  glBufferData(GL_ARRAY_BUFFER,
               grid_positions.size() * sizeof(grid_positions[0]),
               grid_positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);

  // Pose
  std::vector<Eigen::Vector3f> pose_positions = {
      Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(1, 0, 0),
      Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(0, 1, 0),
      Eigen::Vector3f(0, 0, 0), Eigen::Vector3f(0, 0, 1)};
  std::vector<Eigen::Vector3f> pose_colors = {
      Eigen::Vector3f(1, 0, 0), Eigen::Vector3f(1, 0, 0),
      Eigen::Vector3f(0, 1, 0), Eigen::Vector3f(0, 1, 0),
      Eigen::Vector3f(0, 0, 1), Eigen::Vector3f(0, 0, 1)};
  glGenVertexArrays(1, &pose_vao_);
  glBindVertexArray(pose_vao_);
  glGenBuffers(2, pose_buffers_);

  // Positions
  glBindBuffer(GL_ARRAY_BUFFER, pose_buffers_[0]);
  glBufferData(GL_ARRAY_BUFFER,
               pose_positions.size() * sizeof(pose_positions[0]),
               pose_positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  // Colors
  glBindBuffer(GL_ARRAY_BUFFER, pose_buffers_[1]);
  glBufferData(GL_ARRAY_BUFFER, pose_colors.size() * sizeof(pose_colors[0]),
               pose_colors.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(1);

  glBindVertexArray(0);

  Update(0);
}

void Scene::DrawGrid() const {
  glBindVertexArray(grid_vao_);
  glLineWidth(1);
  glDrawArrays(GL_LINES, 0, (kGridSize * 2 + 1) * 4);
  glBindVertexArray(0);
}

void Scene::DrawPose() const {
  glBindVertexArray(pose_vao_);
  glLineWidth(2);
  glDrawArrays(GL_LINES, 0, 6);
  glBindVertexArray(0);
}

const std::vector<std::shared_ptr<Entity>>& Scene::GetEntities() const {
  return entities_;
}

void Scene::AddEntity(
    std::shared_ptr<Entity> entity) {
  entity->SetScene(shared_from_this());
  added_entities_.push_back(entity);
}

void Scene::AddEntities(std::vector<std::shared_ptr<Entity>> entities) {
  for (auto entity : entities) {
    added_entities_.push_back(entity);
  }
}

void Scene::RemoveEntity(
    std::shared_ptr<Entity> entity) {
  removed_entities_.push_back(entity);
}

void Scene::RemoveEntities(std::vector<std::shared_ptr<Entity>> entities) {
  for (auto entity : entities) {
    removed_entities_.push_back(entity);
  }
}
}  // namespace pxl
