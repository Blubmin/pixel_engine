#pragma once

#include <atomic>
#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

namespace pixel_engine {
struct GameState {
  std::string game_name;
  GLFWwindow* window;
  std::atomic<int> window_width;
  std::atomic<int> window_height;
};

class Game {
 public:
  Game(const std::string& game_name);
  ~Game();

  virtual void Init() = 0;
  virtual void Loop() = 0;

  void Run();

  static GameState State;
  static void WindowResizeCallback(GLFWwindow* window, int width, int height);
};
}  // namespace pixel_engine
