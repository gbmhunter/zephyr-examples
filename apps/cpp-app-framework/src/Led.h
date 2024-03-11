#pragma once

#include <zephyr/kernel.h>

#include "StateMachine.h"

class Led : public StateMachine {
    public:
        Led(z_thread_stack_element * threadStack,  void (*threadFnAdapter)(void *, void *, void *));

    private:
        State off;
        // StateMachine sm;

        void StateOffEntryFn();
};