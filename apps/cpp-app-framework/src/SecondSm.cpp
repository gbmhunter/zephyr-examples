#include <zephyr/logging/log.h>

#include "App.hpp"
#include "SecondSm.hpp"

LOG_MODULE_REGISTER(SecondSm, LOG_LEVEL_DBG);

SecondSm::SecondSm(z_thread_stack_element * threadStack,
                   uint32_t threadStackSize_B,
                   void (*threadFnAdapter)(void *, void *, void *),
                   App * app)
    :
        StateMachine(10, threadStack, threadStackSize_B, threadFnAdapter, app->getSmc(), "SecondSm"),
        app(app),
        root(
            std::bind(&SecondSm::Root_Entry, this),
            std::bind(&SecondSm::Root_Event, this, std::placeholders::_1),
            std::bind(&SecondSm::Root_Exit, this),
            nullptr, "Root")
{
    LOG_INF("SecondSM created.");

    helloEventId = app->getSmc()->registerEvent();
    LOG_DBG("Got helloEventId: %d", helloEventId);

    initialTransition(&root);
}

//============================================================
// STATE: Root
//============================================================

void SecondSm::Root_Entry() {
    LOG_INF("Root_Entry");

    // Start LED flashing
    app->getLedSm()->blink(5, 100, 100);
}

void SecondSm::Root_Event(Event* event) {
    LOG_INF("Root_Event");
}

void SecondSm::Root_Exit() {
    LOG_INF("Root_Exit");
}
