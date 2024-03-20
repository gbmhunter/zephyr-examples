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
    Timer1ExpiryEvent() : Event(TypeID::value<Timer1ExpiryEvent>(), "MasterSm::Timer1ExpiryEvent")
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
            nullptr, "Root")
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

    // Turn LED on
    // OnEvent onEvent;
    // app->getLedSm()->sendEvent2(&onEvent, sizeof(onEvent));

    // Send event to second SM
    // app->eventInfo->printHelloEvent->id

    // PrintHelloEvent event;
    // LOG_DBG("event.id: %d", event.id);
    // event.someData = 0;
    // app->getSecondSm()->sendEvent2(&event, sizeof(event));

    // Setup some timers
    Timer1ExpiryEvent timer1ExpiryEvent;
    timer1.start(1*1000, -1, timer1ExpiryEvent);
}

void MasterSm::Root_Event(Event* event) {
    LOG_INF("Root_Event");

    if (event->id == (uint8_t)TypeID::value<Timer1ExpiryEvent>()) {
        LOG_INF("timer1 expired.");
    }
}

void MasterSm::Root_Exit() {
    LOG_INF("Root_Exit");
}
