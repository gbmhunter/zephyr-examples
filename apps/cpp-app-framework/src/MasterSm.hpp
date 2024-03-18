#pragma once

#include "StateMachineLibrary/StateMachine.h"

#include "App.hpp"

class MasterSm : public StateMachine {
public:
    MasterSm(
        z_thread_stack_element * threadStack,
        uint32_t threadStackSize_B,
        void (*threadFnAdapter)(void *, void *, void *),
        App * app);

private:
    App * app;
    State root;
    void Root_Entry();
    void Root_Event(Event* event);
    void Root_Exit();
};