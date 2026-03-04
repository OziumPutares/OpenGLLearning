#include <glad/glad.h>   //
                         //
                         //
#include <GLFW/glfw3.h>  //
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <shape/Buffer.hpp>
#include <shape/Color.hpp>
#include <shape/Drawer.hpp>
#include <shape/Errors.hpp>
#include <shape/Point.hpp>
#include <shape/Rectangle.hpp>
#include <shape/Shader.hpp>
#include <shape/Vector.hpp>

namespace gl {
namespace {

// Window dimensions
constexpr unsigned kStartingWidth = 800;
constexpr unsigned kStartingHeight = 600;

}  // namespace
}  // namespace gl
constexpr auto kStartingScaleFactor = 1.0F / 2;

auto main() -> int {
  // Resize callback function
  // NOLINTNEXTLINE
  auto FramebufferSizeCallback = [](GLFWwindow* /*window*/, int width,
                                    int height) {
    glViewport(0, 0, width, height);
  };
  // Initialize GLFW
  if (glfwInit() == 0) {
    std::cerr << "Failed to initialize GLFW\n";
    return -1;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);  // NOLINT
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

  // Create window
  GLFWwindow* Window =
      glfwCreateWindow(gl::kStartingWidth, gl::kStartingHeight,
                       "Resizable OpenGL Window", nullptr, nullptr);
  if (Window == nullptr) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }

  // Make context current
  glfwMakeContextCurrent(Window);

  // Set the resize callback
  glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);

  // Initialize GLAD
  if (gladLoadGL() == 0) {
    std::cerr << "Failed to initialize GLAD\n";
    return -1;
  }

  glDebugMessageCallback(gl::errors::DebugCallback, nullptr);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  constexpr auto kNumOfVert = 6;
  constexpr auto kPositions = std::array{
      gl::Point{.m_Position = {{-1.0F, -1.0F, 0.0F}},
                .m_Color = {.red = {1.0F}, .alpha = {1.0F}}},
      gl::Point{.m_Position = {{0.0F, -1.0F, 0.0F}},
                .m_Color = {.red = {0.0F}, .blue = {1.0F}, .green = {0.0F}}},
      gl::Point{.m_Position = {{0.0F, -0.7F, 0.0F}},
                .m_Color = {.red = {0.0F}, .blue = {0.0F}, .green = {1.0F}}},
      gl::Point{.m_Position = {{-1.0F, -0.7F, 0.0F}},
                .m_Color = {.red = {0.0F}, .blue = {1.0F}, .alpha = {0.2F}}}};

  constexpr auto kIndices = std::array{0U, 1U, 2U, 0U, 2U, 3U};
  unsigned int buffer;  // NOLINT
  unsigned int EBO{};
  unsigned int VAO = 0;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  gl::BufferData(gl::BufferType::kArray, kPositions, gl::Usage::kStaticDraw);
  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  gl::BufferData(gl::BufferType::kElementArray, kIndices,
                 gl::Usage::kStaticDraw);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gl::Point), nullptr);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(gl::Point),
                        // NOLINTNEXTLINE
                        reinterpret_cast<void*>(sizeof(gl::Vector3<float>)));
  glEnableVertexAttribArray(1);
  auto Result = CreateShaderFromFile(std::filesystem::current_path() / "glsl" /
                                         "newBaseVertexShader.vert.glsl",
                                     std::filesystem::current_path() / "glsl" /
                                         "ourColourFragmentShader.frag.glsl");
  if (!Result.has_value()) {
    Result.error().Handle();
    return (2);
  }
  int const OffsetVertexLocation = glGetUniformLocation(*Result, "offset");
  glUseProgram(*Result);
  glUniform4f(OffsetVertexLocation, 1.0F, 1.0F, 0.0F, 0.0F);

  constexpr auto kNumOfSquaresOnChessBoard = 64UZ;
  std::array<gl::Quadrilateral, kNumOfSquaresOnChessBoard> ListOfSquares{};
  auto constexpr static kCanvasWidth = 2.0F;
  auto Transform = [](auto num) -> auto {
    auto BasePos =
        gl::Vector3<float>{// NOLINTNEXTLINE
                           -1.0F + 0.25F * (num % 8),
                           // NOLINTNEXTLINE
                           -1.0F + 0.25F * static_cast<int>(num / 8), 0.0F};

    auto RGBValue = 0.0F;

    auto constexpr static kWidth = 8Z;
    if ((num + num / kWidth) % 2 == 0) {
      RGBValue = 1.0F;
    } else {
    }
    auto Color = gl::ColorFloat{
        .red = {RGBValue}, .blue = {RGBValue}, .green = {RGBValue}};
    return gl::Quadrilateral(std::array{
        gl::Point{.m_Position{BasePos}, .m_Color{Color}},
        gl::Point{
            .m_Position = {(BasePos + gl::Vector3<float>{kCanvasWidth / kWidth,
                                                         0.0F, 0.0F})},
            .m_Color{Color}},
        gl::Point{
            .m_Position = {(BasePos + gl::Vector3<float>{kCanvasWidth / kWidth,
                                                         kCanvasWidth / kWidth,
                                                         0.0F})},
            .m_Color{Color}},
        gl::Point{.m_Position = {(
                      BasePos +
                      gl::Vector3<float>{0.0F, kCanvasWidth / kWidth, 0.0F})},
                  .m_Color{Color}}});
  };
  for (auto&& [Num, Square] : ListOfSquares | std::views::enumerate) {
    Square = Transform(Num);
  }

  gl::DrawerClass ClearDrawer(
      []([[maybe_unused]] GLFWwindow const& window,
         [[maybe_unused]] std::chrono::nanoseconds deltaTime) -> void {
        // NOLINTNEXTLINE
        glClearColor(0.2F, 0.3F, 0.3F, 1.0F);
        glClear(GL_COLOR_BUFFER_BIT);
      });
  gl::DrawerClass GridDrawer(
      [&VAO, CurrentOffset = gl::Vector2<float>{0.0F, 0.0F},
       KXScalingFactor = kStartingScaleFactor,
       KYScalingFactor = kStartingScaleFactor, &OffsetVertexLocation](
          [[maybe_unused]] GLFWwindow const& window,
          [[maybe_unused]] std::chrono::nanoseconds deltaTime) mutable -> void {
        // NOLINTNEXTLINE
        auto DeltaSec = static_cast<float>(deltaTime.count()) / 1'000'000'000;
        CurrentOffset =
            (gl::Vector2<float>{1.0F * KXScalingFactor * DeltaSec,
                                1.0F * KYScalingFactor * DeltaSec}) +
            CurrentOffset;
        if (CurrentOffset.X() > 1.0F) {
          CurrentOffset.X() = 2 - CurrentOffset.X();
          KXScalingFactor *= -1;
        }
        if (CurrentOffset.X() < 0.0F) {
          CurrentOffset.X() = -CurrentOffset.X();
          KXScalingFactor *= -1;
        }
        // NOLINTNEXTLINE
        if (CurrentOffset.Y() > 1.7F) {
          // NOLINTNEXTLINE
          CurrentOffset.Y() = 3.4F - CurrentOffset.Y();
          KYScalingFactor *= -1;
        }
        if (CurrentOffset.Y() < 0.0F) {
          CurrentOffset.Y() = -CurrentOffset.Y();
          KYScalingFactor *= -1;
        }
        glUniform4f(OffsetVertexLocation, CurrentOffset.X(), CurrentOffset.Y(),
                    0.0F, 0.0F);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, kNumOfVert, GL_UNSIGNED_INT, nullptr);
      });
  auto PreviousTime = std::chrono::system_clock::now();
  // Main loop
  while (glfwWindowShouldClose(Window) == 0) {
    auto const StartTime = std::chrono::system_clock::now();
    std::chrono::nanoseconds const DeltaTimeNano = StartTime - PreviousTime;
    PreviousTime = StartTime;
    // Clear screen
    ClearDrawer.Draw(*Window, DeltaTimeNano);
    std::ranges::for_each(ListOfSquares,
                          [DeltaTimeNano](gl::Quadrilateral& val) -> void {
                            val.Draw(DeltaTimeNano);
                          });
    GridDrawer.Draw(*Window, DeltaTimeNano);

    // Swap buffers and poll events
    glfwSwapBuffers(Window);
    glfwPollEvents();
  }
  glDeleteBuffers(1, &buffer);
  glDeleteProgram(*Result);
  glDeleteVertexArrays(1, &VAO);

  // Clean up
  glfwTerminate();
  return 0;
}
