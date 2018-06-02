#include <pixel_engine/game.h>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
//#include <glog/logging.h>

namespace pixel_engine {
Game::Game(const std::string& game_name) : game_name_(game_name) {
  if (!glfwInit()) {
  }
}
}  // namespace pixel_engine
