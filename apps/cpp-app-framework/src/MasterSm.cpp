#include <zephyr/logging/log.h>

#include "App.hpp"
#include "MasterSm.hpp"
#include "SecondSm.hpp"

LOG_MODULE_REGISTER(MasterSm, LOG_LEVEL_DBG);

//================================================================================================//
// FILE SCOPE EVENTS
//================================================================================================//

#pragma pack(push, 1)
class Timer1ExpiryEvent : public Event {
public:
    Timer1ExpiryEvent() 
        :
        Event(EventId::value<Timer1ExpiryEvent>(), "MasterSm::Timer1ExpiryEvent")
    {
        // nothing to do
    }
};

class Timer2ExpiryEvent : public Event {
public:
    Timer2ExpiryEvent() 
        :
        Event(EventId::value<Timer2ExpiryEvent>(), "MasterSm::Timer2ExpiryEvent")
    {
        // nothing to do
    }
};
#pragma pack(pop)

//================================================================================================//
// STATE MACHINE DEFINITION
//================================================================================================//

MasterSm::MasterSm(z_thread_stack_element * threadStack,
                   uint32_t threadStackSize_B,
                   void (*threadFnAdapter)(void *, void *, void *),
                   App * app)
    :
        StateMachine(10, threadStack, threadStackSize_B, threadFnAdapter, app->getSmc(), "MasterSm"),
        app(app),
        root(
            std::bind(&MasterSm::Root_Entry, this),
            std::bind(&MasterSm::Root_Event, this, std::placeholders::_1),
            std::bind(&MasterSm::Root_Exit, this),
            nullptr, "Root"),
        childStateA(
            std::bind(&MasterSm::ChildStateA_Entry, this),
            std::bind(&MasterSm::ChildStateA_Event, this, std::placeholders::_1),
            std::bind(&MasterSm::ChildStateA_Exit, this),
            &root, "ChildStateA"),
        childStateB(
            std::bind(&MasterSm::ChildStateB_Entry, this),
            std::bind(&MasterSm::ChildStateB_Event, this, std::placeholders::_1),
            std::bind(&MasterSm::ChildStateB_Exit, this),
            &root, "ChildStateB")
{
    LOG_INF("Master SM created.");

    // Register timers
    registerTimer(&timer1);
    registerTimer(&timer2);

    setInitialTransition(&root);
}

//============================================================
// STATE: Root
//============================================================

void MasterSm::Root_Entry() {
    LOG_INF("Root_Entry");

    // Start 2 timers
    Timer1ExpiryEvent timer1ExpiryEvent;
    timer1.start(3*1000, -1, &timer1ExpiryEvent, sizeof(timer1ExpiryEvent));

    Timer2ExpiryEvent timer2ExpiryEvent;
    timer2.start(6*1000, -1, &timer2ExpiryEvent, sizeof(timer2ExpiryEvent));
}

void MasterSm::Root_Event(Event* event) {
    LOG_INF("Root_Event");

    if (event->id == (uint8_t)EventId::value<Timer1ExpiryEvent>()) {
        LOG_INF("timer1 expired.");
        queueTransition(&childStateA);
    } else if (event->id == (uint8_t)EventId::value<Timer2ExpiryEvent>()) {
        LOG_INF("timer2 expired.");
        queueTransition(&childStateB);
    }
}

void MasterSm::Root_Exit() {
    LOG_INF("Root_Exit");
}

//============================================================
// STATE: Root/ChildStateA
//============================================================

void MasterSm::ChildStateA_Entry() {
    LOG_INF("%s called.", __PRETTY_FUNCTION__);
}

void MasterSm::ChildStateA_Event(Event* event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);
}

void MasterSm::ChildStateA_Exit() {
    LOG_INF("%s called.", __PRETTY_FUNCTION__);
}

//============================================================
// STATE: Root/ChildStateB
//============================================================

void MasterSm::ChildStateB_Entry() {
    LOG_INF("%s called.", __PRETTY_FUNCTION__);
}

void MasterSm::ChildStateB_Event(Event* event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);
}

void MasterSm::ChildStateB_Exit() {
    LOG_INF("%s called.", __PRETTY_FUNCTION__);
}