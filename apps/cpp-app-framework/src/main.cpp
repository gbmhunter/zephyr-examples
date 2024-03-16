#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "Led.h"
#include "StateMachine.h"

LOG_MODULE_REGISTER(Main, LOG_LEVEL_DBG);

const uint32_t LED_THREAD_STACK_SIZE_B = 1024;

K_THREAD_STACK_DEFINE(ledThreadStack, LED_THREAD_STACK_SIZE_B);

Led * l_led = nullptr;

void ledThreadFnAdapter(void *, void *, void *) {
    l_led->threadFn();
}

int main(void) {

    auto led = Led(ledThreadStack, LED_THREAD_STACK_SIZE_B, &ledThreadFnAdapter);
    l_led = &led;
    led.start();
    // auto sm = StateMachine(10);

    // printf("Hello, world!\n");
    k_msleep(1000);
    led.turnOn();

    k_msleep(1000);

    // Make the LED flash
    led.blink(5, 1000, 1000);
    
    LOG_DBG("Terminating thread\n");
    led.terminateThread();
    LOG_DBG("Joining thread\n");
    led.join();
    LOG_DBG("main() returning...\n");
}

