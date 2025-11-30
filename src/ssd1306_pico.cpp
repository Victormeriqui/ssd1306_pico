#include "ssd1306_pico.hpp"

#include "default_fonts.hpp"
#include "util.hpp"

#include "etl/delegate.h"
#include "etl/set.h"
#include "etl/string.h"
#include "etl/to_string.h"
#include "hardware/i2c.h"
#include <cmath>
#include <cstdint>

#define MAX_FORMATTED_STRING_SIZE 100

namespace ssd1306_pico
{

    SSD1306::SSD1306(const SSD1306Config& config) : _display_controller(config), _framebuffer(false)
    {
        _display_controller.initialize();
    }

    void SSD1306::fill()
    {
        _framebuffer.fill();
        _framebuffer_has_updates = true;
    }

    void SSD1306::clear()
    {
        _framebuffer.clear();
        _framebuffer_has_updates = true;
    }

    void SSD1306::render()
    {
        _render_iteration++;

        if (!_framebuffer_has_updates)
            return;

        _display_controller.display_framebuffer(_framebuffer);
        _framebuffer_has_updates = false;
    }

    DisplayController<128, 64>& SSD1306::get_display_controller()
    {
        return _display_controller;
    }

    uint8_t SSD1306::get_screen_width() const
    {
        return 128;
    }

    uint8_t SSD1306::get_screen_height() const
    {
        return 64;
    }

    void SSD1306::draw_pixel(uint8_t x, uint8_t y)
    {
        _framebuffer.draw_pixel(x, y);
        _framebuffer_has_updates = true;
    }

    void SSD1306::erase_pixel(uint8_t x, uint8_t y)
    {
        _framebuffer.erase_pixel(x, y);
        _framebuffer_has_updates = true;
    }

    void SSD1306::draw_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
    {
        for (uint8_t cur_x = x; cur_x < get_screen_width() && cur_x < (x + width); cur_x++)
        {
            for (uint8_t cur_y = y; cur_y < get_screen_height() && cur_y < (y + height); cur_y++)
            {
                draw_pixel(cur_x, cur_y);
            }
        }
    }

    void SSD1306::draw_rect_outline(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t thickness)
    {
        draw_rect(x, y, width, thickness);
        draw_rect(x, y, thickness, height);
        draw_rect(x + width, y, thickness, height + thickness);
        draw_rect(x, y + height, width + thickness, thickness);
    }

    void SSD1306::draw_line(uint8_t start_x, uint8_t start_y, uint8_t end_x, uint8_t end_y)
    {
        uint8_t delta_x = std::abs(end_x - start_x);
        uint8_t delta_y = std::abs(end_y - start_y);

        int8_t step_x = (start_x < end_x) ? 1 : -1;
        int8_t step_y = (start_y < end_y) ? 1 : -1;

        int8_t err = ((delta_x > delta_y) ? delta_x : -delta_y) / 2;
        int8_t err2;

        while (start_x != end_x || start_y != end_y)
        {
            draw_pixel(start_x, start_y);

            err2 = err;

            if (err2 > -delta_x)
            {
                err -= delta_y;
                start_x += step_x;
            }
            if (err2 < delta_y)
            {
                err += delta_x;
                start_y += step_y;
            }
        }
    }

    void SSD1306::draw_circle(uint8_t center_x, uint8_t center_y, float radius, uint8_t quality)
    {
        float ang  = 0.0f;
        float step = (M_PI * 2.0f) / quality;

        uint8_t last_x = center_x + radius;
        uint8_t last_y = center_y;

        for (ang = step; ang < M_PI * 2.0f + step; ang += step)
        {
            uint8_t cur_x = center_x + std::cos(ang) * radius;
            uint8_t cur_y = center_y + std::sin(ang) * radius;

            draw_line(last_x, last_y, cur_x, cur_y);

            last_x = cur_x;
            last_y = cur_y;
        }
    }

    void SSD1306::draw_bitmap(uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y, uint8_t map_width, uint8_t map_height, const Bitmap& bitmap)
    {
        _framebuffer.draw_bitmap(x, y, map_x, map_y, map_width, map_height, bitmap);
        _framebuffer_has_updates = true;
    }

    void SSD1306::draw_bitmap(uint8_t x, uint8_t y, const Bitmap& bitmap)
    {
        draw_bitmap(x, y, 0, 0, bitmap.get_width(), bitmap.get_height(), bitmap);
    }

    void SSD1306::draw_bitmap_centered(uint8_t x, uint8_t y, const Bitmap& bitmap)
    {
        draw_bitmap(x - bitmap.get_width() / 2, y - bitmap.get_height() / 2, 0, 0, bitmap.get_width(), bitmap.get_height(), bitmap);
    }

    void SSD1306::draw_bitmap_centered(uint8_t x, uint8_t y, uint8_t map_x, uint8_t map_y, uint8_t map_width, uint8_t map_height, const Bitmap& bitmap)
    {
        draw_bitmap(x - bitmap.get_width() / 2, y - bitmap.get_height() / 2, map_x, map_y, map_width, map_height, bitmap);
    }

    void SSD1306::set_font_size(FontSize size)
    {
        _current_font_size = size;
    }

    FontSize SSD1306::get_font_size() const
    {
        return _current_font_size;
    }

    const Font& SSD1306::_get_font() const
    {
        switch (_current_font_size)
        {
        case FontSize::SMALL:
            return small_font;
        case FontSize::MEDIUM:
            return medium_font;
        case FontSize::LARGE:
            return large_font;
        }

        return medium_font;
    }

    void SSD1306::draw_char(uint8_t x, uint8_t y, char chr)
    {
        const Font& cur_font = _get_font();
        const Bitmap& bitmap = cur_font.get_font_map();

        chr = chr - ' ' + cur_font.get_glyph_offset();
        if (cur_font.is_number_only())
            chr = chr - ('0' - ' ');

        uint8_t glyph_w = cur_font.get_glyph_width();
        uint8_t glyph_h = cur_font.get_glyph_height();

        uint8_t chars_per_row = cur_font.get_font_map_width() / glyph_w;
        uint8_t glyph_y       = static_cast<uint8_t>(chr) / chars_per_row * glyph_h;
        uint8_t glyph_x       = (chr % chars_per_row) * glyph_w;

        draw_bitmap(x, y, glyph_x, glyph_y, glyph_w, glyph_h, bitmap);
    }

    void SSD1306::draw_string(uint8_t x, uint8_t y, etl::string_view str)
    {
        const Font& cur_font = _get_font();

        uint8_t glyph_w = cur_font.get_glyph_width();
        uint8_t glyph_h = cur_font.get_glyph_height();

        uint8_t cur_x = x;
        uint8_t cur_y = y;

        for (char chr : str)
        {
            draw_char(cur_x, cur_y, chr);
            cur_x += glyph_w;

            if (cur_x + glyph_w > get_screen_width())
            {
                cur_y += glyph_h;
                cur_x = x;
            }
        }
    }

    void SSD1306::draw_string_centered(uint8_t x, uint8_t y, etl::string_view str)
    {
        const Font& cur_font = _get_font();

        uint8_t glyph_w = cur_font.get_glyph_width();
        uint8_t glyph_h = cur_font.get_glyph_height();

        uint8_t str_width = str.size() * glyph_w;

        draw_string(x - str_width / 2, y - glyph_h / 2, str);
    }

    void SSD1306::draw_string(uint8_t x, uint8_t y, int32_t num)
    {
        const Font& cur_font = _get_font();

        uint8_t glyph_w = cur_font.get_glyph_width();
        uint8_t glyph_h = cur_font.get_glyph_height();

        uint8_t cur_x = x;
        uint8_t cur_y = y;

        uint8_t remaining_digits = get_digit_count(num);
        int16_t div              = (int16_t)std::pow(10, remaining_digits - 1);

        while (remaining_digits > 0)
        {
            uint8_t digit = (num / div) % 10;

            draw_char(cur_x, cur_y, '0' + digit);
            cur_x += glyph_w;

            if (cur_x + glyph_w > get_screen_width())
            {
                cur_y += glyph_h;
                cur_x = x;
            }

            div /= 10;
            remaining_digits--;
        }
    }

    void SSD1306::draw_string_centered(uint8_t x, uint8_t y, int32_t num)
    {
        const Font& cur_font = _get_font();

        uint8_t glyph_w = cur_font.get_glyph_width();
        uint8_t glyph_h = cur_font.get_glyph_height();

        uint8_t str_width = get_digit_count(num) * glyph_w;

        draw_string(x - str_width / 2, y - glyph_h / 2, num);
    }

    void SSD1306::draw_string_formatted(uint8_t x, uint8_t y, etl::string_view str, ...)
    {
        static etl::set<char, 10> SPECIAL_CHARS = {'%', '\n'};
        static etl::string<MAX_FORMATTED_STRING_SIZE> STR_BUFF;

        const Font& cur_font = _get_font();
        uint8_t glyph_w      = cur_font.get_glyph_width();
        uint8_t glyph_h      = cur_font.get_glyph_height();

        uint8_t cur_x = x;
        uint8_t cur_y = y;

        va_list arglist;
        va_start(arglist, str);

        for (size_t i = 0; i < str.size(); i++)
        {
            char chr = str[i];

            // handle line wrapping
            if (cur_x + glyph_w > get_screen_width())
            {
                cur_y += glyph_h;
                cur_x = x;
            }

            // normal character
            if (!SPECIAL_CHARS.contains(chr))
            {
                draw_char(cur_x, cur_y, chr);
                cur_x += glyph_w;
                continue;
            }

            // formatted character
            switch (chr)
            {
            case '\n':
                cur_y += glyph_h;
                cur_x = x;
                continue;
            case '%':
                char chr_next = str[++i];    // skip next character as it's part of the format specifier
                switch (chr_next)
                {
                case 'c':
                    draw_char(cur_x, cur_y, va_arg(arglist, int));
                    cur_x += glyph_w;
                    break;
                case 'd':
                case 'i':
                    etl::to_string(va_arg(arglist, int), STR_BUFF);
                    draw_string(cur_x, cur_y, STR_BUFF);
                    cur_x += glyph_w * STR_BUFF.size();
                    break;
                case 'x':
                    etl::to_string(va_arg(arglist, int), STR_BUFF, etl::format_spec().hex());
                    draw_string(cur_x, cur_y, STR_BUFF);
                    cur_x += glyph_w * STR_BUFF.size();
                    break;
                case 'f':
                    etl::to_string(va_arg(arglist, double), STR_BUFF, etl::format_spec().precision(2));
                    draw_string(cur_x, cur_y, STR_BUFF);
                    cur_x += glyph_w * STR_BUFF.size();
                    break;
                case 's':
                    STR_BUFF = va_arg(arglist, const char*);
                    draw_string(cur_x, cur_y, STR_BUFF);
                    cur_x += glyph_w * STR_BUFF.size();
                    break;
                case '%':
                    draw_char(cur_x, cur_y, '%');
                    cur_x += glyph_w;
                    break;
                }
            }
        }
    }

    void SSD1306::erase_rect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
    {
        for (uint8_t cur_x = x; cur_x < get_screen_width() && cur_x < (x + width); cur_x++)
        {
            for (uint8_t cur_y = y; cur_y < get_screen_height() && cur_y < (y + height); cur_y++)
            {
                erase_pixel(cur_x, cur_y);
            }
        }
    }

    void SSD1306::blink_section(uint8_t blink_frequency, uint8_t blink_period, etl::delegate<void()> filled_draw_call, etl::delegate<void()> unfilled_draw_call)
    {
        if ((_render_iteration % blink_period) < blink_frequency)
            filled_draw_call();
        else
            unfilled_draw_call();
    }
}    // namespace ssd1306_pico