#include <cstdio>

#include <zephyr/logging/log.h>

#include "Led.h"

LOG_MODULE_REGISTER(Led, LOG_LEVEL_DBG);

const uint8_t MAX_NUM_STATES = 10;

Led::Led(z_thread_stack_element * threadStack,  void (*threadFnAdapter)(void *, void *, void *)) :
        sm(MAX_NUM_STATES, threadStack, 1024, threadFnAdapter),
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


    sm.addState(&off);
    sm.addTimer(&timer);

    // Setup initial transition
    sm.initialTransition(&off);
}

void Led::turnOn() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);

    // Send event to state machine
    sm.sendEvent(Event((uint8_t)LedEventId::ON, nullptr));
}

void Led::terminateThread() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);

    // Send event to state machine
    sm.sendEvent(Event((uint8_t)LedEventId::TERMINATE_THREAD, nullptr));
}

//============================================================
// STATE: Root
//============================================================

void Led::Root_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

void Led::Root_Event(Event event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event.id);


    if (event.id == (uint8_t)LedEventId::TERMINATE_THREAD) {
        sm.terminateThreadSm();
        return;
    }

    return;
}

void Led::Root_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

//============================================================
// STATE: Root/Off
//============================================================

void Led::Off_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

void Led::Off_Event(Event event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event.id);

    if (event.id == (uint8_t)LedEventId::ON) {
        // Transition to On state
        // transitionTo(&on);
        
        // result.nextState = &on;
        // EventFnResult<LedEvent> result;
        // result.nextState(&on);
        sm.queueTransition(&on);
        return;
    }
    return;
}

void Led::Off_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

//============================================================
// STATE: Root/On
//============================================================

void Led::On_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);

    // Start timer
    Event event;
    event.id = (uint8_t)LedEventId::TIMER_EXPIRED;
    timer.start(k_ms_to_ticks_floor64(5*1000), event);
}

void Led::On_Event(Event event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event.id);
    return;
}

void Led::On_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}