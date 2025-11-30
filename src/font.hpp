#pragma once

#include "bitmap.hpp"

#include <cstdint>

namespace ssd1306_pico {
class Font {
public:
  Font(uint8_t glyph_width, uint8_t glyph_height, uint8_t glyph_offset,
       uint8_t font_map_width, uint8_t _font_map_height, const Bitmap &buffer,
       bool is_number_only = false);
  Font(const Font &font) = delete;
  Font(Font &&font) = delete;
  Font &operator=(const Font &font) = delete;
  Font &operator=(Font &&font) = delete;
  ~Font() = default;

  [[nodiscard]] uint8_t get_font_map_width() const;
  [[nodiscard]] uint8_t get_font_map_height() const;

  [[nodiscard]] bool is_number_only() const;
  [[nodiscard]] uint8_t get_glyph_width() const;
  [[nodiscard]] uint8_t get_glyph_height() const;
  [[nodiscard]] uint8_t get_glyph_offset() const;

  [[nodiscard]] const Bitmap &get_font_map() const;

private:
  uint8_t _glyph_width;
  uint8_t _glyph_height;
  uint8_t _glyph_offset;

  uint8_t _font_map_width;
  uint8_t _font_map_height;

  Bitmap _font_map;

  bool _is_number_only = false;
};

} // namespace ssd1306_pico