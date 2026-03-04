
#include <GLFW/glfw3.h>

#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <memory>
#include <variant>
namespace gl {

struct DrawingConcept {
  DrawingConcept() = default;
  DrawingConcept(DrawingConcept const&) = default;
  DrawingConcept(DrawingConcept&&) = default;
  auto operator=(DrawingConcept const&) -> DrawingConcept& = default;
  auto operator=(DrawingConcept&&) -> DrawingConcept& = default;
  virtual void Draw(GLFWwindow const& window, std::chrono::nanoseconds) = 0;
  virtual ~DrawingConcept() = default;
};

template <typename Func>
  requires std::is_invocable_v<Func, GLFWwindow const&,
                               std::chrono::nanoseconds>
class ConcreteDrawer : DrawingConcept {
  Func m_drawStrategy{};

 public:
  explicit ConcreteDrawer(Func function)
      : m_drawStrategy(std::move(function)) {}
  void Draw(GLFWwindow const& window,
            std::chrono::nanoseconds deltaTime) override {
    m_drawStrategy(window, deltaTime);
  }
};
constexpr std::size_t kBytesForStackDrawer{128};
class DrawerClass {
  std::variant<std::unique_ptr<DrawingConcept>,
               std::array<std::byte, kBytesForStackDrawer>>
      m_concreteDrawHolder;

 public:
  template <typename Func>
    requires std::invocable<Func, GLFWwindow const&, std::chrono::nanoseconds>
  explicit DrawerClass(Func drawingFunction) {
    if constexpr (sizeof(ConcreteDrawer<Func>) <= kBytesForStackDrawer) {
      m_concreteDrawHolder = std::array<std::byte, kBytesForStackDrawer>{};
      std::construct_at(
          // NOLINTNEXTLINE
          reinterpret_cast<ConcreteDrawer<Func>*>(std::addressof(
              std::get<std::array<std::byte, kBytesForStackDrawer>>(
                  m_concreteDrawHolder))),
          // NOLINTNEXTLINE
          ConcreteDrawer<Func>(drawingFunction));
    } else {
      m_concreteDrawHolder.emplace(
          std::unique_ptr(ConcreteDrawer(drawingFunction)));
    }
  }
  void Draw(GLFWwindow const& window, std::chrono::nanoseconds deltaTime) {
    if (std::holds_alternative<std::unique_ptr<DrawingConcept>>(
            m_concreteDrawHolder)) {
      std::get<std::unique_ptr<DrawingConcept>>(m_concreteDrawHolder)
          ->Draw(window, deltaTime);
    } else {
      // NOLINTNEXTLINE
      reinterpret_cast<DrawingConcept*>(
          &std::get<std::array<std::byte, kBytesForStackDrawer>>(
              m_concreteDrawHolder))
          ->Draw(window, deltaTime);
    }
  }
};
}  // namespace gl
