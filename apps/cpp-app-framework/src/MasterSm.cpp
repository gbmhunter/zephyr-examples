#include <zephyr/logging/log.h>

#include "App.hpp"
#include "MasterSm.hpp"
#include "SecondSm.hpp"

LOG_MODULE_REGISTER(MasterSm, LOG_LEVEL_DBG);

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

    initialTransition(&root);
}

//============================================================
// STATE: Root
//============================================================

void MasterSm::Root_Entry() {
    LOG_INF("Root_Entry");

    // Start LED flashing
    app->getLedSm()->blink(5, 100, 100);

    // Send event to second SM
    // app->eventInfo->printHelloEvent->id

    PrintHelloEvent event;
    LOG_DBG("event.id: %d", event.id);
    event.someData = 0;
    app->getSecondSm()->sendEvent2(&event, sizeof(event));
}

void MasterSm::Root_Event(Event* event) {
    LOG_INF("Root_Event");
}

void MasterSm::Root_Exit() {
    LOG_INF("Root_Exit");
}
