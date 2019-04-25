#pragma once

#include <atomic>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace pxl {
struct GameState {
  std::string game_name;
  GLFWwindow* window;
  std::atomic<int> window_width;
  std::atomic<int> window_height;

  float GetAspectRatio();
};

class Game {
 public:
  Game(const std::string& game_name);
  ~Game();

  virtual void Init() = 0;
  virtual void Update(float time_elapsed) = 0;

  void Run();

  static GameState State;
  static void WindowResizeCallback(GLFWwindow* window, int width, int height);
};
}  // namespace pxl
