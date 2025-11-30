#include "font.hpp"

#include <utility>

namespace ssd1306_pico {

Font::Font(uint8_t glyph_width, uint8_t glyph_height, uint8_t glyph_offset,
           uint8_t font_map_width, uint8_t _font_map_height,
           const Bitmap &buffer, bool is_number_only)
    : _glyph_width(glyph_width), _glyph_height(glyph_height),
      _glyph_offset(glyph_offset), _font_map_width(font_map_width),
      _font_map_height(_font_map_height), _font_map(buffer),
      _is_number_only(is_number_only) {}

uint8_t Font::get_font_map_width() const { return _font_map_width; }

uint8_t Font::get_font_map_height() const { return _font_map_height; }

uint8_t Font::get_glyph_width() const { return _glyph_width; }

uint8_t Font::get_glyph_height() const { return _glyph_height; }

uint8_t Font::get_glyph_offset() const { return _glyph_offset; }

const Bitmap &Font::get_font_map() const { return _font_map; }

bool Font::is_number_only() const { return _is_number_only; }

} // namespace ssd1306_pico