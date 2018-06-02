#pragma once

#include <string>

namespace pixel_engine {
class Game {
 public:
  Game(const std::string& game_name);

  virtual void Init() = 0;
  virtual void Loop() = 0;

 private:
  std::string game_name_;
};
}  // namespace pixel_engine
