#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "App.hpp"
#include "Gpio/GpioMock.hpp"

LOG_MODULE_REGISTER(Main, LOG_LEVEL_DBG);

int main(void)
{
    GpioMock gpioMock;
    auto app = App(&gpioMock);
    app.run();
    LOG_DBG("main() returning...\n");
}

