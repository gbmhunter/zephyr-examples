#include <stdbool.h>
#include <stdint.h>

#include <zephyr/drivers/gpio.h>

#define RGB_LED_INIT_VALUE 0x64FAA3B2

typedef enum {
    RGB_LED_STATUS_OK = 0,
    RGB_LED_STATUS_ERROR = 1,
} RgbLed_Status_t;

typedef struct {
    uint32_t initialized;

    gpio_dt_spec * redLedGpio;
    gpio_dt_spec * greenLedGpio;
    gpio_dt_spec * blueLedGpio;
} RgbLed_t;

RgbLed_Status_t RgbLed_Init(RgbLed_t * me, gpio_dt_spec * redLedGpio, gpio_dt_spec * greenLedGpio, gpio_dt_spec * blueLedGpio);
