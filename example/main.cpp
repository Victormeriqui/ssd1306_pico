#include "etl/string.h"
#include "pico/stdlib.h"
#include "ssd1306_config.hpp"
#include "ssd1306_pico.hpp"

using namespace ssd1306_pico;

int main()
{
    const uint led_pin = 25;
    stdio_init_all();
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    SSD1306Config config {
        .i2c_instance = i2c_default,
        .sda_pin      = 6,
        .scl_pin      = 7,
        .i2c_address  = 0x3C,
    };

    SSD1306 oled(config);

    while (true)
    {
        gpio_put(led_pin, true);
        oled.clear();

        oled.draw_rect(0, 0, 10, 10);
        oled.draw_rect_outline(12, 0, 10, 10, 2);
        oled.draw_line(28, 0, 40, 10);
        oled.draw_circle(56, 10, 8, 15);

        oled.set_font_size(FontSize::SMALL);
        oled.draw_string(80, 20, 12345);
        oled.draw_string_formatted(0, 30, "f string lf\nint: %d char: %c hex: %x lf\nstring: %s escape: %%", 123, 'm', 57005, "hello");

        oled.set_font_size(FontSize::MEDIUM);
        oled.draw_string(0, 50, "medium font");

        oled.render();
        sleep_ms(1000);
        gpio_put(led_pin, false);
        sleep_ms(1000);
    }
}