#include <pixel_engine/game.h>

#include <chrono>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glog/logging.h>
#include <boost/format.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <imgui/examples/imgui_impl_glfw.h>
#include <imgui/examples/imgui_impl_opengl3.h>

namespace pxl {
namespace {
void GLFWErrorCallback(int error, const char *description) {
  LOG(FATAL) << error << ":" << description;
}
}  // namespace

float GameState::GetAspectRatio() {
  return window_width / float(window_height);
}

GameState Game::State;

Game::Game(const std::string &game_name) {
  State.game_name = game_name;
  State.window = nullptr;

  glfwSetErrorCallback(GLFWErrorCallback);
  if (glfwInit() != GLFW_TRUE) {
    CHECK(false) << "glfwInit failed.";
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  State.window =
      glfwCreateWindow(1280, 720, State.game_name.c_str(), NULL, NULL);
  if (State.window == nullptr) {
    CHECK(false) << "glfwCreateWindow failed.";
  }
  glfwMakeContextCurrent(State.window);

  if (gl3wInit()) {
    LOG(FATAL) << "gl3wInit failed";
  }
  if (!gl3wIsSupported(4, 5)) {
    LOG(FATAL) << "OpenGL 4.5 not supported";
  }
  const std::string glsl_version = "#version 450";

  LOG(INFO) << "OpenGL " << glGetString(GL_VERSION) << ", GLSL "
            << glGetString(GL_SHADING_LANGUAGE_VERSION);

  /** Init OpenGL Defaults **/
  glfwSwapInterval(1);
  glClearColor(.2f, .2f, .2f, 1.f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA,
                      GL_DST_ALPHA);
  glBlendEquation(GL_FUNC_ADD);

  /* Set Viewport */
  int width;
  int height;
  glfwGetFramebufferSize(State.window, &width, &height);
  State.window_width = width;
  State.window_height = height;
  glViewport(0, 0, width, height);
  glfwSetWindowSizeCallback(State.window, Game::WindowResizeCallback);

  /* Setup ImGui context */
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard
  // Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable
  // Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(State.window, true);
  ImGui_ImplOpenGL3_Init(glsl_version.c_str());
}

Game::~Game() {}

void Game::Run() {
  Init();
  glClearColor(.2, .2, .2, 1);

  auto time = std::chrono::system_clock::now();

  while (!glfwWindowShouldClose(State.window)) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Run implemented game loop
    auto now = std::chrono::system_clock::now();
    auto time_spent = now - time;
    Update(std::chrono::duration_cast<
               std::chrono::duration<float, std::ratio<1, 1>>>(time_spent)
               .count());
    time = now;

    // Show fps
    boost::format fps_format("%.02lf fps");
    fps_format % ImGui::GetIO().Framerate;
    ImVec2 fps_location =
        ImVec2(State.window_width -
                   ImGui::CalcTextSize(fps_format.str().c_str()).x,
               0) +
        ImVec2(-10, 10);
    ImGui::GetOverlayDrawList()->AddText(fps_location, IM_COL32_WHITE,
                                         fps_format.str().c_str());

    // Rendering
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(State.window);

    if (glfwGetKey(State.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(State.window, GLFW_TRUE);
    }
  }
}

void Game::WindowResizeCallback(GLFWwindow *window, int width, int height) {
  State.window_width = width;
  State.window_height = height;
  glViewport(0, 0, width, height);
}
}  // namespace pxl
