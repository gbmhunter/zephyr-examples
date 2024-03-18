#include <zephyr/logging/log.h>

#include "StateMachineController.h"

LOG_MODULE_REGISTER(StateMachineController, LOG_LEVEL_INF);

void StateMachineController::registerStateMachine(StateMachine * stateMachine)
{
    LOG_DBG("Registering state machine \"%s\".", stateMachine->getName());
    __ASSERT_NO_MSG(numStateMachines + 1 <= MAX_NUM_STATE_MACHINES);
    stateMachines[numStateMachines] = stateMachine;
    numStateMachines++;
}

void StateMachineController::startAll()
{
    LOG_DBG("Starting %u state machines...", numStateMachines);
    for (uint8_t i = 0; i < numStateMachines; i++)
    {
        stateMachines[i]->start();
    }
}