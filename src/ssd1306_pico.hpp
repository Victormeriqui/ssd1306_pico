#pragma once

#include "bitmap.hpp"
#include "display_controller.hpp"
#include "font.hpp"
#include "framebuffer.hpp"
#include "ssd1306_config.hpp"

#include "etl/delegate.h"
#include "etl/string.h"
#include <cstdint>

namespace ssd1306_pico
{
    enum class FontSize
    {
        SMALL,
        MEDIUM,
        LARGE
    };

    class SSD1306
    {
    public:
        SSD1306(const SSD1306Config& config);
        SSD1306(const SSD1306& ssd1306)            = delete;
        SSD1306(SSD1306&& ssd1306)                 = delete;
        SSD1306& operator=(const SSD1306& ssd1306) = delete;
        SSD1306& operator=(SSD1306&& ssd1306)      = delete;
        ~SSD1306()                                 = default;

        void fill();
        void clear();
        void render();

        [[nodiscard]] DisplayController<128, 64>& get_display_controller();

        [[nodiscard]] uint8_t get_screen_width() const;
        [[nodiscard]] uint8_t get_screen_height() const;

        void draw_pixel(uint8_t x, uint8_t y);
        void erase_pixel(uint8_t x, uint8_t y);

        void draw_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
        void draw_rect_outline(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t thickness);

        void draw_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y);
        void draw_circle(uint8_t center_x, uint8_t center_y, float radius, uint8_t quality);

        void draw_bitmap(uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y, uint8_t map_width, uint8_t map_height, const Bitmap& bitmap);
        void draw_bitmap(uint8_t x, uint8_t y, const Bitmap& bitmap);
        void draw_bitmap_centered(uint8_t x, uint8_t y, const Bitmap& bitmap);
        void draw_bitmap_centered(uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y, uint8_t map_width, uint8_t map_height, const Bitmap& bitmap);

        void set_font_size(FontSize size);
        [[nodiscard]] FontSize get_font_size() const;

        void draw_char(uint8_t x, uint8_t y, char chr);
        void draw_string(uint8_t x, uint8_t y, etl::string_view str);
        void draw_string_centered(uint8_t x, uint8_t y, etl::string_view str);
        void draw_string(uint8_t x, uint8_t y, int32_t num);
        void draw_string_centered(uint8_t x, uint8_t y, int32_t num);
        void draw_string_formatted(uint8_t x, uint8_t y, etl::string_view str, ...);

        void erase_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

        void blink_section(uint8_t blink_frequency, uint8_t blink_period, etl::delegate<void()> filled_draw_call, etl::delegate<void()> unfilled_draw_call);

    private:
        const Font& _get_font() const;

    private:
        DisplayController<128, 64> _display_controller;
        FrameBuffer<128, 64> _framebuffer;
        bool _framebuffer_has_updates = false;

        FontSize _current_font_size = FontSize::MEDIUM;

        uint8_t _render_iteration = 0;
    };
}    // namespace ssd1306_pico