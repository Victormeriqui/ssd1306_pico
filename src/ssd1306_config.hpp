#pragma once

#include "hardware/i2c.h"
#include <cstdint>

namespace ssd1306_pico
{
    struct SSD1306Config
    {
        i2c_inst_t* i2c_instance;
        uint8_t sda_pin;
        uint8_t scl_pin;
        uint8_t i2c_address;
    };

}    // namespace ssd1306_pico