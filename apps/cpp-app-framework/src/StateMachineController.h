#pragma once

#include "StateMachine.h"

const uint8_t MAX_NUM_STATE_MACHINES = 20;

class StateMachineController
{
public:
    void registerStateMachine(StateMachine * stateMachine)
    {
        stateMachines[numStateMachines] = stateMachine;
        numStateMachines++;
    }

    uint8_t registerEvent()
    {
        numEvents++;
        return numEvents - 1;
    }

    void postAll(Event * event);
    void post(Event * event, StateMachine * stateMachine);

private:
    StateMachine * stateMachines[MAX_NUM_STATE_MACHINES];
    uint8_t numStateMachines = 0;

    uint8_t numEvents = 0;
};