#include <glog/logging.h>
#include <pixel_engine/game.h>

class HelloGame : public pixel_engine::Game {
 public:
  HelloGame() : pixel_engine::Game("Hello Game") {}
  void Init() override {}
  void Loop() override {
    LOG(INFO) << Game::State.window_width << ", " << Game::State.window_height;
  }
};

int main(int argc, char *argv[]) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);
  HelloGame game;
  game.Run();
  return 0;
}
