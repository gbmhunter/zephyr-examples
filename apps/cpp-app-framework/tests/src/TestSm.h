
#pragma once

#include <cstring>

#include <zephyr/kernel.h>

#include "StateMachine.h"

const uint8_t CALL_STACK_DEPTH = 10;

enum class TestSmEventId {
    TEST_EVENT_1 = (uint8_t)EventId::MAX_VALUE,
    ROOT_EVENT,
};

class TestSm {
    public:
        TestSm(
            z_thread_stack_element * threadStack, 
            void (*threadFnAdapter)(void *, void *, void *));

        void turnOn();

        StateMachine sm;

        std::array<const char *, CALL_STACK_DEPTH> callstack = { nullptr };

        void addToCallstack(const char * functionName);

        std::array<const char *, CALL_STACK_DEPTH> getCallstackAndClear();

        void fireTestEvent1();

        void fireRootEvent();

    private:

        uint8_t callstackIdx = 0;

        State root;
        void Root_Entry();
        void Root_Event(Event event);
        void Root_Exit();

        State state1;
        void State1_Entry();
        void State1_Event(Event event);
        void State1_Exit();

        State state2;
        void State2_Entry();
        void State2_Event(Event event);
        void State2_Exit();

        Timer timer;
};