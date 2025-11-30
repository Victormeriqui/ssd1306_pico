#pragma once

#include <cstdint>

namespace ssd1306_pico {
class Bitmap {
public:
  Bitmap(uint8_t width, uint8_t height, bool filled = false);
  Bitmap(uint8_t width, uint8_t height, const uint8_t *data);
  Bitmap(const Bitmap &bitmap);
  Bitmap(Bitmap &&bitmap) = default;
  Bitmap &operator=(const Bitmap &bitmap) = delete;
  Bitmap &operator=(Bitmap &&bitmap) = delete;
  ~Bitmap();

  [[nodiscard]] const uint8_t *get_data() const;
  [[nodiscard]] uint8_t get_width() const;
  [[nodiscard]] uint8_t get_height() const;

  void fill();
  void clear();
  void draw_pixel(uint8_t x, uint8_t y);
  void erase_pixel(uint8_t x, uint8_t y);

private:
  uint8_t _width;
  uint8_t _height;
  uint8_t *_data;
};
} // namespace ssd1306_pico