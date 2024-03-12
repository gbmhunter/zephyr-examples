#include <cstdio>

#include <zephyr/logging/log.h>

#include "Led.h"

LOG_MODULE_REGISTER(Led, LOG_LEVEL_DBG);

Led::Led(z_thread_stack_element * threadStack,  void (*threadFnAdapter)(void *, void *, void *)) :
        StateMachine(1, threadStack, 1024, threadFnAdapter),
        off(std::bind(&Led::Off_Entry, this))
    {
    LOG_INF("Led created\n");


    addState(&off);

    // Setup initial transition
    initialTransition(&off);

    run();
}

void Led::turnOn() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);

    // Send event to state machine
    sendEvent(1);
}

void Led::Off_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

void Led::Off_Event(LedEvent event) {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

void Led::Off_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}