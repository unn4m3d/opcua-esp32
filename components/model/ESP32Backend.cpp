#include "ESP32Backend.hpp"

using namespace model;

void ESP32Backend::gpio_mode(ESP32Backend::pin_id_t pin, ESP32Backend::pin_mode_t mode, bool pu, bool pd)
{
    gpio_config_t cfg{
        .pin_bit_mask = 1ull << pin,
        .mode = mode,
        .pull_up_en = (pu ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE),
        .pull_down_en = (pd ? GPIO_PULLDOWN_ENABLE : GPIO_PULLDOWN_DISABLE),
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&cfg);
}

void ESP32Backend::gpio_write(ESP32Backend::pin_id_t pin, uint32_t value)
{
    gpio_set_level(pin, value);
}

uint32_t ESP32Backend::gpio_read(ESP32Backend::pin_id_t pin)
{
    return gpio_get_level(pin);
}
