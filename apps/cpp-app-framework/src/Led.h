
#pragma once

#include <cstring>

#include <zephyr/kernel.h>

#include "StateMachine.h"

enum class LedEventId {
    ON = (uint8_t)EventId::MAX_VALUE,
    OFF,
    TIMER_EXPIRED,
    MAX_VALUE,
};

class LedEvent {
public:
    LedEventId id;
    uint8_t data[10];

    LedEvent() {
        this->id = LedEventId::OFF;
        memset(this->data, 0, 10);
    }

    LedEvent(LedEventId id, const char * data) {
        this->id = id;
        if (data != nullptr) {
            memcpy(this->data, data, 10);
        }
    }
};

class Led {
    public:
        Led(z_thread_stack_element * threadStack,  void (*threadFnAdapter)(void *, void *, void *));

        void turnOn();

        StateMachine sm;

    private:


        State root;
        void Root_Entry();
        void Root_Event(Event event);
        void Root_Exit();

        State off;
        void Off_Entry();
        void Off_Event(Event event);
        void Off_Exit();

        State on;
        void On_Entry();
        void On_Event(Event event);
        void On_Exit();

        Timer timer;
};