#pragma once

#include <zephyr/kernel.h>

#include "StateMachine.h"

enum class LedEventId {
    ON,
    OFF
};

class LedEvent {
    LedEventId id;
    char data[10];
};

class Led : public StateMachine {
    public:
        Led(z_thread_stack_element * threadStack,  void (*threadFnAdapter)(void *, void *, void *));

        void turnOn();

    private:
        State off;
        // StateMachine sm;

        void Off_Entry();
        void Off_Event(LedEvent event);
        void Off_Exit();
};