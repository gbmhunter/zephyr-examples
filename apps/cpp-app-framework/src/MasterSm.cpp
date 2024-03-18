#include <zephyr/logging/log.h>

#include "MasterSm.hpp"

LOG_MODULE_REGISTER(MasterSm, LOG_LEVEL_DBG);

MasterSm::MasterSm(z_thread_stack_element * threadStack,
                   uint32_t threadStackSize_B,
                   void (*threadFnAdapter)(void *, void *, void *),
                   StateMachineController * smc)
    :
        StateMachine(10, threadStack, threadStackSize_B, threadFnAdapter, smc),
        root(
            std::bind(&MasterSm::Root_Entry, this),
            std::bind(&MasterSm::Root_Event, this, std::placeholders::_1),
            std::bind(&MasterSm::Root_Exit, this),
            nullptr, "Root")
{
    LOG_INF("Master SM created.");
}