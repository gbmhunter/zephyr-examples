#pragma once

#include "StateMachineLibrary/StateMachine.hpp"

#include "App.hpp"

class PrintHelloEvent : public Event {
public:
    uint8_t someData;
    PrintHelloEvent() : Event(EventId::value<PrintHelloEvent>(), "SecondSm::PrintHelloEvent")
    {
        someData = 0;
    }
};

class SecondSm : public StateMachine {
public:
    uint8_t helloEventId;
    SecondSm(
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