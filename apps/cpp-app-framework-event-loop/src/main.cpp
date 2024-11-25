#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>


LOG_MODULE_REGISTER(Main, LOG_LEVEL_DBG);

int main(void)
{
    LOG_DBG("main() returning...\n");
}

