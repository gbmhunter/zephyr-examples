#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>

#include "Led.h"
#include "StateMachine.h"

K_THREAD_STACK_DEFINE(ledThreadStack, 1024);

Led * l_led = nullptr;

void ledThreadFnAdapter(void *, void *, void *) {
    printf("ledThreadFnAdapter\n");
    l_led->threadFn();
}

int main(void) {

    auto led = Led(ledThreadStack, &ledThreadFnAdapter);
    l_led = &led;
    led.start();
    // auto sm = StateMachine(10);

    // printf("Hello, world!\n");
    k_msleep(1000);
    led.turnOn();

    k_msleep(1000);

    printf("Terminating thread\n");
    led.terminateThread();
    printf("Joining thread\n");
    led.join();
    printf("main() returning...\n");

    return 0;
}
