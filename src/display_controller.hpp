#pragma once

#include "framebuffer.hpp"
#include "register_defines.hpp"
#include "ssd1306_config.hpp"

#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <algorithm>

namespace ssd1306_pico
{
    template<uint8_t WIDTH, uint8_t HEIGHT>
    class DisplayController
    {
    public:
        DisplayController(SSD1306Config config, bool external_vcc = false);
        DisplayController(const DisplayController& controller)            = delete;
        DisplayController(DisplayController&& controller)                 = delete;
        DisplayController& operator=(const DisplayController& controller) = delete;
        DisplayController& operator=(DisplayController&& controller)      = delete;
        ~DisplayController()                                              = default;

        void initialize();
        void display_framebuffer(const FrameBuffer<WIDTH, HEIGHT>& framebuffer);
        void set_invesion(bool inverted);
        void set_dimming(bool dimmed);
        void set_contrast(uint8_t contrast);

    private:
        void _send_command(uint8_t command);
        void _send_data(const uint8_t* data, uint8_t length);

    private:
        SSD1306Config _config;
        bool _is_external_vcc;
    };

    template<uint8_t WIDTH, uint8_t HEIGHT>
    DisplayController<WIDTH, HEIGHT>::DisplayController(SSD1306Config config, bool external_vcc) : _config(std::move(config)), _is_external_vcc(external_vcc)
    {
    }

    template<uint8_t WIDTH, uint8_t HEIGHT>
    void DisplayController<WIDTH, HEIGHT>::_send_command(uint8_t command)
    {
        uint8_t control_and_cmd[2] = {0x80, command};
        i2c_write_blocking(_config.i2c_instance, _config.i2c_address, control_and_cmd, 2, false);
    }

    template<uint8_t WIDTH, uint8_t HEIGHT>
    void DisplayController<WIDTH, HEIGHT>::_send_data(const uint8_t* data, uint8_t length)
    {
        static constexpr size_t MAX_I2C_TRANSFER              = 64;
        static uint8_t CONTROL_AND_DATA[MAX_I2C_TRANSFER + 1] = {0x40};

        std::copy(data, data + length, CONTROL_AND_DATA + 1);

        i2c_write_blocking(_config.i2c_instance, _config.i2c_address, CONTROL_AND_DATA, length + 1, false);
    }

    template<uint8_t WIDTH, uint8_t HEIGHT>
    void DisplayController<WIDTH, HEIGHT>::initialize()
    {
        i2c_init(_config.i2c_instance, 400 * 1000);
        gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
        gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
        gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

        _send_command(SSD1306_DISPLAYOFF);            // 0xAE
        _send_command(SSD1306_SETDISPLAYCLOCKDIV);    // 0xD5
        _send_command(0x80);                          // the suggested ratio 0x80
        _send_command(SSD1306_SETMULTIPLEX);          // 0xA8
        _send_command(0x3F);
        _send_command(SSD1306_SETDISPLAYOFFSET);      // 0xD3
        _send_command(0x0);                           // no offset
        _send_command(SSD1306_SETSTARTLINE | 0x0);    // line #0
        _send_command(SSD1306_CHARGEPUMP);            // 0x8D
        _send_command(_is_external_vcc ? 0x10 : 0x14);

        _send_command(SSD1306_MEMORYMODE);        // 0x20
        _send_command(0x00);                      // 0x0 horizontal addressing
        _send_command(SSD1306_SEGREMAP | 0x1);    // 0xA0
        _send_command(SSD1306_COMSCANDEC);        // 0xC8
        _send_command(SSD1306_SETCOMPINS);        // 0xDA
        _send_command(0x12);
        _send_command(SSD1306_SETCONTRAST);    // 0x81
        _send_command(_is_external_vcc ? 0x9F : 0xCF);
        _send_command(SSD1306_SETPRECHARGE);    // 0xd9
        _send_command(_is_external_vcc ? 0x22 : 0xF1);
        _send_command(SSD1306_SETVCOMDETECT);    // 0xDB
        _send_command(0x40);

        _send_command(SSD1306_DEACTIVATE_SCROLL);      // 0x2E
        _send_command(SSD1306_DISPLAYALLON_RESUME);    // 0xA4
        _send_command(SSD1306_NORMALDISPLAY);          // 0xA6

        _send_command(SSD1306_DISPLAYON);
    }

    template<uint8_t WIDTH, uint8_t HEIGHT>
    void DisplayController<WIDTH, HEIGHT>::display_framebuffer(const FrameBuffer<WIDTH, HEIGHT>& framebuffer)
    {
        const uint8_t* data = framebuffer.get_data();

        for (uint8_t page = 0; page < 8; page++)
        {
            _send_command(0x10);           // set higher column address
            _send_command(0x00);           // set lower column address
            _send_command(0xB0 + page);    // set page address

            _send_data(data + page * WIDTH, 64);
            _send_data(data + page * WIDTH + 64, 64);
        }
    }

    template<uint8_t WIDTH, uint8_t HEIGHT>
    void DisplayController<WIDTH, HEIGHT>::set_invesion(bool inverted)
    {
        if (inverted)
            _send_command(SSD1306_INVERTDISPLAY);
        else
            _send_command(SSD1306_NORMALDISPLAY);
    }

    template<uint8_t WIDTH, uint8_t HEIGHT>
    void DisplayController<WIDTH, HEIGHT>::set_dimming(bool dimmed)
    {
        uint8_t contrast;

        if (dimmed)
            contrast = 0;    // Dimmed display
        else
            contrast = _is_external_vcc ? 0x9F : 0xCF;

        _send_command(SSD1306_SETCONTRAST);
        _send_command(contrast);
    }

    template<uint8_t WIDTH, uint8_t HEIGHT>
    void DisplayController<WIDTH, HEIGHT>::set_contrast(uint8_t contrast)
    {
        _send_command(SSD1306_SETCONTRAST);
        _send_command(contrast);
    }

}    // namespace ssd1306_pico
