#pragma once

#include "StateMachineLibrary/StateMachine.hpp"

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

    State childStateA;
    void ChildStateA_Entry();
    void ChildStateA_Event(Event* event);
    void ChildStateA_Exit();

    State childStateB;
    void ChildStateB_Entry();
    void ChildStateB_Event(Event* event);
    void ChildStateB_Exit();

    Timer timer1;
    Timer timer2;
};