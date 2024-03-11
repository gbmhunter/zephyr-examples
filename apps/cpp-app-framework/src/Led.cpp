#include <cstdio>

#include <zephyr/logging/log.h>

#include "Led.h"

LOG_MODULE_REGISTER(Led, LOG_LEVEL_DBG);

Led::Led(z_thread_stack_element * threadStack,  void (*threadFnAdapter)(void *, void *, void *)) :
        StateMachine(1, threadStack, 1024, threadFnAdapter),
        off(std::bind(&Led::StateOffEntryFn, this))
    {
    LOG_INF("Led created\n");


    addState(&off);

    // Setup initial transition
    initialTransition(&off);

    run();
}

void Led::StateOffEntryFn() {
    printf("StateOffEventFn1\n");
}
