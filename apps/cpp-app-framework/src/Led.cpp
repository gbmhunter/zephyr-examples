#include <cstdio>

#include <zephyr/logging/log.h>

#include "Led.h"

LOG_MODULE_REGISTER(Led, LOG_LEVEL_DBG);

const uint8_t MAX_NUM_STATES = 10;

Led::Led(z_thread_stack_element * threadStack,  void (*threadFnAdapter)(void *, void *, void *)) :
        StateMachine(MAX_NUM_STATES, threadStack, 1024, threadFnAdapter),
        root(
            std::bind(&Led::Root_Entry, this),
            std::bind(&Led::Root_Event, this, std::placeholders::_1),
            std::bind(&Led::Root_Exit, this),
            nullptr, "Root"),
        off(
            std::bind(&Led::Off_Entry, this),
            std::bind(&Led::Off_Event, this, std::placeholders::_1),
            std::bind(&Led::Off_Exit, this),
            &root, "Off"),
        on(
            std::bind(&Led::On_Entry, this),
            std::bind(&Led::On_Event, this, std::placeholders::_1),
            std::bind(&Led::On_Exit, this),
            &root, "On")
    {
    LOG_INF("Led created\n");


    addState(&off);
    addTimer(&timer);

    // Setup initial transition
    initialTransition(&off);
}

void Led::turnOn() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);

    // Send event to state machine
    sendEvent(LedEvent(LedEventId::ON, nullptr));
}

//============================================================
// STATE: Root
//============================================================

void Led::Root_Entry() {
    LOG_INF("%s() called", __PRETTY_FUNCTION__);
}

void Led::Root_Event(LedEvent event) {
    LOG_INF("%s() called", __PRETTY_FUNCTION__);
}

void Led::Root_Exit() {
    LOG_INF("%s() called", __PRETTY_FUNCTION__);
}

//============================================================
// STATE: Off
//============================================================

void Led::Off_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

void Led::Off_Event(LedEvent event) {
    LOG_INF("%s called", __PRETTY_FUNCTION__);

    if (event.id == LedEventId::ON) {
        // Transition to On state
        transitionTo(&on);
    }
}

void Led::Off_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

//============================================================
// STATE: On
//============================================================

void Led::On_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);

    // Start timer
    LedEvent event;
    event.id = LedEventId::TIMER_EXPIRED;
    timer.start(k_ms_to_ticks_floor64(5*1000), event);
}

void Led::On_Event(LedEvent event) {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

void Led::On_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}