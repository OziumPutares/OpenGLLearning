#include <fmt/base.h>
#include <fmt/format.h>

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <utility>
namespace error_handling {

enum struct ErrorLevel : std::uint8_t {
  Notification,
  Warning,
  Error,
  CriticalError,
};
struct State {
  std::string m_Message;
  ErrorLevel m_Level;

  // NOLINTNEXTLINE
  State(std::string what, ErrorLevel level)
      : m_Message(std::move(what)), m_Level(level) {}
};
}  // namespace error_handling
template <typename T>
using ErrorHandler = std::expected<T, error_handling::State>;

template <>
struct fmt::formatter<error_handling::State> : formatter<std::string_view> {
  auto format(error_handling::State const& value, format_context& ctx) const
      -> format_context::iterator {
    return formatter<std::string_view>::format(
        fmt::format("Message: {}\n", value.m_Message), ctx);
  }
};
