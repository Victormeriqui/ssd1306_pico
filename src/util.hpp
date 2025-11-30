#pragma once

#include <cmath>
#include <cstdint>

namespace ssd1306_pico {

[[nodiscard]] static uint8_t get_digit_count(int32_t value) {
  return ((uint8_t)std::log10(value)) + 1;
}
} // namespace ssd1306_pico