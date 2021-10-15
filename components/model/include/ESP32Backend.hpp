#pragma once

#include "driver/gpio.h"

namespace model
{
    struct ESP32Backend
    {
        typedef gpio_num_t pin_id_t; 
        typedef gpio_mode_t pin_mode_t;

        static void gpio_mode(pin_id_t, pin_mode_t, bool pu, bool pd);
        static void gpio_write(pin_id_t, uint32_t value);
        static uint32_t gpio_read(pin_id_t);
        // TODO
    };

    typedef ESP32Backend DefaultBackend;
}