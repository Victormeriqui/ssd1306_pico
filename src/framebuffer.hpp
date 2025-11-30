#pragma once

#include <algorithm>
#include <cstdint>

#include "bitmap.hpp"

namespace ssd1306_pico {
template <uint8_t WIDTH, uint8_t HEIGHT> class FrameBuffer {
public:
  FrameBuffer(bool filled = false);
  FrameBuffer(const uint8_t *data);
  FrameBuffer(const FrameBuffer &framebuffer) = delete;
  FrameBuffer(FrameBuffer &&framebuffer) = delete;
  FrameBuffer &operator=(const FrameBuffer &framebuffer) = delete;
  FrameBuffer &operator=(FrameBuffer &&framebuffer) = delete;
  ~FrameBuffer() = default;

  [[nodiscard]] const uint8_t *get_data() const;

  void fill();
  void clear();
  void draw_pixel(uint8_t x, uint8_t y);
  void erase_pixel(uint8_t x, uint8_t y);

  void draw_bitmap(uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y,
                   uint8_t map_width, uint8_t map_height, const Bitmap &bitmap);

private:
  uint8_t _data[WIDTH * (HEIGHT / 8)];
};

template <uint8_t WIDTH, uint8_t HEIGHT>
FrameBuffer<WIDTH, HEIGHT>::FrameBuffer(bool filled) {
  if (filled)
    fill();
  else
    clear();
}

template <uint8_t WIDTH, uint8_t HEIGHT>
FrameBuffer<WIDTH, HEIGHT>::FrameBuffer(const uint8_t *data) {
  std::copy(data, data + (WIDTH * (HEIGHT / 8)), _data);
}

template <uint8_t WIDTH, uint8_t HEIGHT>
const uint8_t *FrameBuffer<WIDTH, HEIGHT>::get_data() const {
  return _data;
}

template <uint8_t WIDTH, uint8_t HEIGHT>
void FrameBuffer<WIDTH, HEIGHT>::fill() {
  std::fill(_data, _data + (WIDTH * (HEIGHT / 8)), 0xFF);
}

template <uint8_t WIDTH, uint8_t HEIGHT>
void FrameBuffer<WIDTH, HEIGHT>::clear() {
  std::fill(_data, _data + (WIDTH * (HEIGHT / 8)), 0x00);
}

template <uint8_t WIDTH, uint8_t HEIGHT>
void FrameBuffer<WIDTH, HEIGHT>::draw_pixel(uint8_t x, uint8_t y) {
  uint8_t segment = x;
  uint8_t page = y / 8;

  uint8_t mask = 1 << (y % 8);
  uint16_t cellpos = segment + page * WIDTH;

  _data[cellpos] |= mask;
}

template <uint8_t WIDTH, uint8_t HEIGHT>
void FrameBuffer<WIDTH, HEIGHT>::erase_pixel(uint8_t x, uint8_t y) {
  uint8_t segment = x;
  uint8_t page = y / 8;

  uint8_t mask = 0xFF - (1 << (y % 8));
  uint16_t cellpos = segment + page * WIDTH;

  _data[cellpos] &= mask;
}

template <uint8_t WIDTH, uint8_t HEIGHT>
void FrameBuffer<WIDTH, HEIGHT>::draw_bitmap(uint8_t x, uint8_t y,
                                             uint8_t map_x, uint8_t map_y,
                                             uint8_t map_width,
                                             uint8_t map_height,
                                             const Bitmap &bitmap) {
  uint8_t screen_x = x;
  uint8_t screen_y = y;
  for (uint8_t cur_x = map_x;
       cur_x < bitmap.get_width() && cur_x < (map_x + map_width); cur_x++) {
    for (uint8_t cur_y = map_y;
         cur_y < bitmap.get_height() && cur_y < (map_y + map_height); cur_y++) {
      uint8_t segment = cur_x;
      uint8_t page = cur_y / 8;
      uint8_t mask = 1 << (cur_y % 8);
      uint16_t cellpos = segment + page * bitmap.get_width();

      if (bitmap.get_data()[cellpos] & mask)
        draw_pixel(screen_x, screen_y++);
      else
        erase_pixel(screen_x, screen_y++);
    }
    screen_x++;
    screen_y = y;
  }
}

} // namespace ssd1306_pico