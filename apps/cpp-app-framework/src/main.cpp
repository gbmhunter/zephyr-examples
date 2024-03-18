#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "App.hpp"

LOG_MODULE_REGISTER(Main, LOG_LEVEL_DBG);

int main(void)
{
    auto app = App();
    app.run();
    LOG_DBG("main() returning...\n");
}

