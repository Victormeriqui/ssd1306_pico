#include "bitmap.hpp"

#include <algorithm>
#include <cstdint>

namespace ssd1306_pico {

Bitmap::Bitmap(uint8_t width, uint8_t height, bool filled)
    : _width(width), _height(height) {
  _data = new uint8_t[_width * _height];
  if (filled)
    fill();
  else
    clear();
}

Bitmap::Bitmap(uint8_t width, uint8_t height, const uint8_t *data)
    : _width(width), _height(height) {
  _data = new uint8_t[_width * _height];
  std::copy(data, data + _width * _height, _data);
}

Bitmap::Bitmap(const Bitmap &bitmap)
    : _width(bitmap._width), _height(bitmap._height) {
  _data = new uint8_t[_width * _height];
  std::copy(bitmap._data, bitmap._data + _width * _height, _data);
}

Bitmap::~Bitmap() { delete[] _data; }

void Bitmap::fill() { std::fill(_data, _data + _width * _height, 0xFF); }

void Bitmap::clear() { std::fill(_data, _data + _width * _height, 0x00); }

void Bitmap::draw_pixel(uint8_t x, uint8_t y) {
  uint8_t segment = x;
  uint8_t page = y / 8;

  uint8_t mask = 1 << (y % 8);
  uint16_t cellpos = segment + page * _width;

  _data[cellpos] |= mask;
}

void Bitmap::erase_pixel(uint8_t x, uint8_t y) {
  uint8_t segment = x;
  uint8_t page = y / 8;

  uint8_t mask = 0xFF - (1 << (y % 8));
  uint16_t cellpos = segment + page * _width;

  _data[cellpos] &= mask;
}

const uint8_t *Bitmap::get_data() const { return _data; }

uint8_t Bitmap::get_width() const { return _width; }

uint8_t Bitmap::get_height() const { return _height; }

} // namespace ssd1306_pico
