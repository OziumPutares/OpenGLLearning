#include <fmt/base.h>

#include <cstddef>
#include <cstdint>
#include <iterator>

namespace {

[[nodiscard]] auto SumValues(uint8_t const *Data, size_t Size) {
  constexpr auto kScale = 1000;

  int Value = 0;
  for (std::size_t Offset = 0; Offset < Size; ++Offset) {
    Value +=
        static_cast<int>(*std::next(Data, static_cast<long>(Offset))) * kScale;
  }
  return Value;
}
}  // namespace

// Fuzzer that attempts to invoke undefined behavior for signed integer overflow
// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
namespace {
extern "C" int LLVMFuzzerTestOneInput(uint8_t const *Data, size_t Size) {
  fmt::print("Value sum: {}, len{}\n", SumValues(Data, Size), Size);
  return 0;
}
}  // namespace
