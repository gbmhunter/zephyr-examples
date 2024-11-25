
#pragma once

#include <cstring>

#include <zephyr/kernel.h>

#include "StateMachineLibrary/StateMachine.hpp"

const uint8_t CALL_STACK_DEPTH = 10;

//================================================================================================//
// PUBLIC EVENTS
//================================================================================================//

class TestEvent1 : public Event {
    public:
        TestEvent1() : Event((uint8_t)EventId::value<TestEvent1>(), "TestSm::TestEvent1") {}
};

class RootEvent : public Event {
    public:
        RootEvent() : Event((uint8_t)EventId::value<RootEvent>(), "TestSm::RootEvent") {}
};

class GotoState2AEvent : public Event {
    public:
        GotoState2AEvent() : Event((uint8_t)EventId::value<GotoState2AEvent>(), "TestSm::GotoState2AEvent") {}
};

class GotoStateRoot2Event : public Event {
    public:
        GotoStateRoot2Event() : Event((uint8_t)EventId::value<GotoStateRoot2Event>(), "TestSm::GotoStateRoot2Event") {}
};

class StopPropagationEvent : public Event {
    public:
        StopPropagationEvent() : Event((uint8_t)EventId::value<StopPropagationEvent>(), "TestSm::StopPropagationEvent") {}
};

//================================================================================================//
// STATE MACHINE PROTOTYPE
//================================================================================================//

class TestSm : public StateMachine {
    public:
        TestSm(
            z_thread_stack_element * threadStack, 
            void (*threadFnAdapter)(void *, void *, void *),
            StateMachineController * smc);

        void turnOn();

        std::array<const char *, CALL_STACK_DEPTH> callstack = { nullptr };

        void addToCallstack(const char * functionName);

        std::array<const char *, CALL_STACK_DEPTH> getCallstackAndClear();

    private:

        uint8_t callstackIdx = 0;

        State root;
        void Root_Entry();
        void Root_Event(Event * event);
        void Root_Exit();

        State state1;
        void State1_Entry();
        void State1_Event(Event * event);
        void State1_Exit();

        State state2;
        void State2_Entry();
        void State2_Event(Event * event);
        void State2_Exit();

        State state2A;
        void State2A_Entry();
        void State2A_Event(Event * event);
        void State2A_Exit();

        State root2;
        void Root2_Entry();
        void Root2_Event(Event * event);
        void Root2_Exit();

        Timer timer;
};