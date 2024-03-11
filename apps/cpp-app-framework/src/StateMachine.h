#pragma once

#include <functional>

#include <zephyr/kernel.h>

class State {
    public:
        State(std::function<void()> stateFn) {
            this->stateFn = stateFn;
        }

        void run() {
            stateFn();
        }

    private:
        std::function<void()> stateFn;
};

class StateMachine {

    public:

    StateMachine(uint8_t maxNumStates, z_thread_stack_element * threadStack, uint32_t threadStackSize_B, void (*threadFnAdapter)(void *, void *, void *));

    void addState(State * state);

    void initialTransition(State * state);

    void run();

    void threadFn();

    private:
        uint8_t maxNumStates;
        uint8_t numStates;
        State ** states;
        State * currentState;

        char * msgQueueBuffer;
        struct k_msgq msgQueue;

        z_thread_stack_element * threadStack;
        struct k_thread thread;

};