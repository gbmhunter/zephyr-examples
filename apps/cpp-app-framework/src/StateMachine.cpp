#include <zephyr/kernel.h>

#include "StateMachine.h"

StateMachine::StateMachine(uint8_t maxNumStates, z_thread_stack_element * threadStack, uint32_t threadStackSize_B, void (*threadFnAdapter)(void *, void *, void *)) {
    this->maxNumStates = maxNumStates;
    this->states = static_cast<State**>(k_malloc(this->maxNumStates * sizeof(State*)));

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

void StateMachine::start() {
    printf("StateMachine::start\n");
    k_thread_start(&this->thread);
}

void StateMachine::addState(State * state) {
    printf("StateMachine::addState\n");
    __ASSERT_NO_MSG(this->numStates < this->maxNumStates);
    this->states[this->numStates] = state;
    this->numStates++;
}

void StateMachine::initialTransition(State * state) {
    printf("StateMachine::initialTransition\n");
    this->currentState = state;
}

void StateMachine::run() {
    printf("StateMachine::run\n");
    this->currentState->run();
}

void StateMachine::threadFn() {
    printf("StateMachine::threadFn\n");
    while (1) {
        printf("StateMachine::threadFn while\n");
        k_msleep(1000);
    }
}

void StateMachine::sendEvent(uint8_t event) {
    k_msgq_put(&msgQueue, &event, K_NO_WAIT);
}
