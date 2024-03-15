
#pragma once

#include <cstring>

#include <zephyr/kernel.h>

#include "StateMachine.h"

const uint8_t CALL_STACK_DEPTH = 10;

enum class TestSmEventId {
    TEST_EVENT_1 = (uint8_t)EventId::MAX_VALUE,
    ROOT_EVENT,
    GOTO_STATE_2A,
    GOTO_STATE_ROOT2,
};

class TestSm : public StateMachine {
    public:
        TestSm(
            z_thread_stack_element * threadStack, 
            void (*threadFnAdapter)(void *, void *, void *));

        void turnOn();

        std::array<const char *, CALL_STACK_DEPTH> callstack = { nullptr };

        void addToCallstack(const char * functionName);

        std::array<const char *, CALL_STACK_DEPTH> getCallstackAndClear();

        void fireTestEvent1();

        void fireRootEvent();

        void gotoState2A();

        void gotoStateRoot2();

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

        State state2A;
        void State2A_Entry();
        void State2A_Event(Event event);
        void State2A_Exit();

        State root2;
        void Root2_Entry();
        void Root2_Event(Event event);
        void Root2_Exit();

        Timer timer;
};