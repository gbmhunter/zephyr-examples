#include <stdbool.h>
#include <stdint.h>

#include "RgbLed.h"

RgbLed_Status_t RgbLed_Init(RgbLed_t * me, gpio_dt_spec * redLedGpio, gpio_dt_spec * greenLedGpio, gpio_dt_spec * blueLedGpio) {
    me->redLedGpio = redLedGpio;
    me->greenLedGpio = greenLedGpio;
    me->blueLedGpio = blueLedGpio;
    
    me->initialized = RGB_LED_INIT_VALUE;
    return RGB_LED_STATUS_OK;
}