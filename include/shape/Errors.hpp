#include <spdlog/fmt/bundled/format.h>

#include <cstdint>
#include <expected>
#include <stacktrace>
#include <string>
namespace error_handling {

enum struct ErrorLevel : std::uint8_t {
  Notification,
  Warning,
  Error,
  CriticalError,
};
struct State {
  std::stacktrace m_BackTrace;
  std::string m_Message;
  ErrorLevel m_Level;

  // NOLINTNEXTLINE
  State(std::string what, ErrorLevel level)
      : m_BackTrace(std::stacktrace::current()),
        m_Message(std::move(what)),
        m_Level(level) {}
};
}  // namespace error_handling
template <typename T>
using ErrorHandler = std::expected<T, error_handling::State>;
template <>
struct fmt::formatter<error_handling::State> {
  static constexpr auto parse(format_parse_context& ctx)
      -> decltype(ctx.begin()) {
    return ctx.end();
  }

  template <typename FormatContext>
  auto format(error_handling::State const& input, FormatContext& ctx)
      -> decltype(ctx.out()) {
#ifdef DEBUG
    return format_to(ctx.out(), "Message: {} Backtrace: {}", input.m_Message,
                     input.m_BackTrace);
#else
    return format_to(ctx.out(), "Message: {}", input.m_Message);
#endif
  }
};
