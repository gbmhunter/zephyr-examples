#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

template<typename T>
class State {
public:
    State(std::function<void()> entryFn, std::function<void(T)> eventFn, std::function<void()> exitFn) {
        this->entryFn = entryFn;
        this->eventFn = eventFn;
        this->exitFn = exitFn;
    }

    std::function<void()> entryFn;
    std::function<void(T)> eventFn;
    std::function<void()> exitFn;
};

template<typename T>
class StateMachine {

public:

    StateMachine(uint8_t maxNumStates, z_thread_stack_element * threadStack, uint32_t threadStackSize_B, void (*threadFnAdapter)(void *, void *, void *)) {
        this->maxNumStates = maxNumStates;
        this->states = static_cast<State<T>**>(k_malloc(this->maxNumStates * sizeof(State<T>*)));

        this->nextState = nullptr;

        // Create message queue for receiving messages
        const uint8_t sizeOfMsg = 10;
        const uint8_t numMsgs = 10;
        this->msgQueueBuffer = static_cast<char*>(k_malloc(numMsgs * sizeOfMsg));
        k_msgq_init(&msgQueue, msgQueueBuffer, 10, 10);

        this->threadStack = threadStack;
        k_tid_t my_tid = k_thread_create(&this->thread, this->threadStack,
                                        threadStackSize_B,
                                        threadFnAdapter,
                                        NULL, NULL, NULL,
                                        5, 0,
                                        K_FOREVER); // Don't start the thread yet

        printf("StateMachine created\n");
    }

    void start() {
        printf("StateMachine::start\n");
        k_thread_start(&this->thread);
    }

    void addState(State<T> * state) {
        printf("StateMachine::addState\n");
        __ASSERT_NO_MSG(this->numStates < this->maxNumStates);
        this->states[this->numStates] = state;
        this->numStates++;
    }

    void initialTransition(State<T> * state)  {
        printf("StateMachine::initialTransition\n");
        this->currentState = state;
    }

    // void run()  {
    //     printf("StateMachine::run\n");
    //     this->currentState->run();
    // }

    void threadFn()  {
        printf("StateMachine::threadFn\n");

        // Perform initial transition
        this->currentState->entryFn();

        while (1) {

            // Block on message queue
            T event;
            k_msgq_get(&msgQueue, &event, K_FOREVER);
            printf("Got message!\n");

            // Call the event handler for the current state
            this->currentState->eventFn(event);

            // If the user has requested a state transition, perform it now
            if (this->nextState != nullptr) {
                this->currentState->exitFn();
                this->currentState = this->nextState;
                this->currentState->entryFn();
                this->nextState = nullptr;
            }
        }
    }

    void sendEvent(T event)  {
        k_msgq_put(&msgQueue, &event, K_NO_WAIT);
    }

    void transitionTo(State<T> * state)  {
        // Save state to transition to
        this->nextState = state;
    }


private:
    uint8_t maxNumStates;
    uint8_t numStates;
    State<T> ** states;
    State<T> * currentState;
    State<T> * nextState;

    char * msgQueueBuffer;
    struct k_msgq msgQueue;

    z_thread_stack_element * threadStack;
    struct k_thread thread;

};