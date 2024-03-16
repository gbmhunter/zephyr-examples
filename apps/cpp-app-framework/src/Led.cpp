#include <cstdio>

#include <zephyr/logging/log.h>

#include "Led.h"

LOG_MODULE_REGISTER(Led, LOG_LEVEL_DBG);

const uint8_t MAX_NUM_STATES = 10;

Led::Led(
    z_thread_stack_element * threadStack,
    uint32_t threadStackSize_B,
    void (*threadFnAdapter)(void *, void *, void *)) :
        StateMachine(MAX_NUM_STATES, threadStack, threadStackSize_B, threadFnAdapter),
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
    auto event = Event((uint8_t)LedEventId::ON);
    sendEvent2(&event, sizeof(event));
}

void Led::blink(uint8_t numTimes, uint32_t onTime_ms, uint32_t offTime_ms)
{
    LOG_INF("%s called", __PRETTY_FUNCTION__);

    // char data[10];
    // BlinkEvent * event = reinterpret_cast<BlinkEvent*>(data);
    // event->id = (uint8_t)LedEventId::BLINK;
    // event->numTimes = numTimes;
    // event->onTime_ms = onTime_ms;
    // event->offTime_ms = offTime_ms;

    BlinkEvent event(numTimes, onTime_ms, offTime_ms);

    // Send event to state machine
    // k_msgq_put(&msgQueue, &data, K_NO_WAIT);
    // auto event1 = Event(event);
    sendEvent2(&event, sizeof(event));
}

//============================================================
// STATE: Root
//============================================================

void Led::Root_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}

void Led::Root_Event(Event* event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);

    if (event->id == (uint8_t)LedEventId::BLINK) {
        // Got blink event. Reinterp cast to BlinkEvent
        auto blinkEvent = reinterpret_cast<BlinkEvent*>(event);

        LOG_DBG("Got blink event. numTimes: %u, onTime_ms: %u, offTime_ms: %u", blinkEvent->numTimes, blinkEvent->onTime_ms, blinkEvent->offTime_ms);
        
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

void Led::Off_Event(Event* event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);

    if (event->id == (uint8_t)LedEventId::ON) {
        // Transition to On state
        queueTransition(&on);
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

void Led::On_Event(Event * event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);
    return;
}

void Led::On_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
}