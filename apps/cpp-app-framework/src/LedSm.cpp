#include <cstdio>

#include <zephyr/logging/log.h>

#include "LedSm.hpp"

LOG_MODULE_REGISTER(LedSm, LOG_LEVEL_DBG);

const uint8_t MAX_NUM_STATES = 10;

LedSm::LedSm(z_thread_stack_element * threadStack,
         uint32_t threadStackSize_B,
         void (*threadFnAdapter)(void *, void *, void *),
         StateMachineController * smc,
         const char * name)
    :
        StateMachine(MAX_NUM_STATES, threadStack, threadStackSize_B, threadFnAdapter, smc, name),
        root(
            std::bind(&LedSm::Root_Entry, this),
            std::bind(&LedSm::Root_Event, this, std::placeholders::_1),
            std::bind(&LedSm::Root_Exit, this),
            nullptr, "Root"),
        off(
            std::bind(&LedSm::Off_Entry, this),
            std::bind(&LedSm::Off_Event, this, std::placeholders::_1),
            std::bind(&LedSm::Off_Exit, this),
            &root, "Off"),
        on(
            std::bind(&LedSm::On_Entry, this),
            std::bind(&LedSm::On_Event, this, std::placeholders::_1),
            std::bind(&LedSm::On_Exit, this),
            &root, "On")
{
    LOG_DBG("Led created\n");

    addState(&off);
    registerTimer(&timer);

    // Setup initial transition
    setInitialTransition(&off);
}

void LedSm::turnOn() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);

    // Send event to state machine
    auto event = OnEvent();
    sendEvent(&event, sizeof(event));
}

void LedSm::blink(uint8_t numTimes, uint32_t onTime_ms, uint32_t offTime_ms)
{
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    BlinkEvent event(numTimes, onTime_ms, offTime_ms);

    // Send event to state machine
    sendEvent(&event, sizeof(event));
}

//============================================================
// STATE: Root
//============================================================

void LedSm::Root_Entry() {
    LOG_DBG("%s called", __PRETTY_FUNCTION__);
}

void LedSm::Root_Event(Event* event) {
    LOG_DBG("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);

    if (event->id == (uint8_t)EventId::value<BlinkEvent>()) {
        // Got blink event. Reinterp cast to BlinkEvent
        auto blinkEvent = reinterpret_cast<BlinkEvent*>(event);

        LOG_DBG("Got blink event. numTimes: %u, onTime_ms: %u, offTime_ms: %u", blinkEvent->numTimes, blinkEvent->onTime_ms, blinkEvent->offTime_ms);
        
        return;
    }

    return;
}

void LedSm::Root_Exit() {
    LOG_DBG("%s called", __PRETTY_FUNCTION__);
}

//============================================================
// STATE: Root/Off
//============================================================

void LedSm::Off_Entry() {
    LOG_DBG("%s called", __PRETTY_FUNCTION__);
}

void LedSm::Off_Event(Event* event) {
    LOG_DBG("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);

    if (event->id == (uint8_t)EventId::value<OnEvent>()) {
        // Transition to On state
        queueTransition(&on);
        return;
    }
    return;
}

void LedSm::Off_Exit() {
    LOG_DBG("%s called", __PRETTY_FUNCTION__);
}

//============================================================
// STATE: Root/On
//============================================================

void LedSm::On_Entry() {
    LOG_DBG("%s called", __PRETTY_FUNCTION__);

    // Start timer
    TimerExpiryEvent event;
    timer.start(k_ms_to_ticks_floor64(5*1000), &event, sizeof(event));
}

void LedSm::On_Event(Event * event) {
    LOG_DBG("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);
    if (event->id == (uint8_t)EventId::value<TimerExpiryEvent>()) {
        // Timer expired
        LOG_DBG("Timer expired.");
        queueTransition(&off);
        return;
    }
    return;
}

void LedSm::On_Exit() {
    LOG_DBG("%s called", __PRETTY_FUNCTION__);
}