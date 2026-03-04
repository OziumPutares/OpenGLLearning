#include <myproject/sample_library.hpp>

int factorial(int input) noexcept {
  int Result = 1;

  while (input > 0) {
    Result *= input;
    --input;
  }

  return Result;
}
