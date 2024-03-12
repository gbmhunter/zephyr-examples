#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

const uint8_t MAX_NUM_NESTED_STATES = 10;

template<typename T>
class Timer {
public:
    int64_t period_ticks;
    int64_t startTime_ticks;
    int64_t timeToNextFire_ticks;
    bool m_isRunning;
    T event;

    Timer() :
        period_ticks(0),
        startTime_ticks(0),
        timeToNextFire_ticks(0),
        m_isRunning(false)
    {
        // nothing to do
    }

    void start(int64_t period_ticks, T event) {
        // Start timer
        this->period_ticks = period_ticks;
        this->event = event;

        // Save current time to work out when next to fire
        this->startTime_ticks = k_uptime_ticks();

        this->timeToNextFire_ticks = this->startTime_ticks + this->period_ticks;

        this->m_isRunning = true;
    }

    bool isRunning() {
        return this->m_isRunning;
    }

    void incrementNextFireTime() {
        this->timeToNextFire_ticks += this->period_ticks;
    }
};

template<typename T>
class State {
public:
    State(
        std::function<void()> entryFn,
        std::function<void(T)> eventFn,
        std::function<void()> exitFn,
        State * parent = nullptr,
        char * name = nullptr) :
        entryFn(entryFn),
        eventFn(eventFn),
        exitFn(exitFn),
        parent(parent),
        name(name)
    {
        // nothing to do
    }

    std::function<void()> entryFn;
    std::function<void(T)> eventFn;
    std::function<void()> exitFn;

    State * parent;
    char * name;
};

template<typename T>
class StateMachine {

public:

    StateMachine(uint8_t maxNumStates, z_thread_stack_element * threadStack, uint32_t threadStackSize_B, void (*threadFnAdapter)(void *, void *, void *)) :
        nextState(nullptr),
        timer(nullptr)
    {
        this->maxNumStates = maxNumStates;
        this->states = static_cast<State<T>**>(k_malloc(this->maxNumStates * sizeof(State<T>*)));


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
        k_thread_start(&this->thread);
    }

    void addState(State<T> * state) {
        __ASSERT(this->numStates < this->maxNumStates, "Exceeded max number of states of %u when trying to add state \"\".", this->maxNumStates, state->name ? state->name : "unknown");
        this->states[this->numStates] = state;
        this->numStates++;
    }

    void addTimer(Timer<T> * timer) {
        this->timer = timer;
    }

    void initialTransition(State<T> * state)  {
        this->currentState = state;
    }

    void threadFn()  {
        printf("Thread function for SM started...\n");

        // Perform initial transition. Call the top-most state entry function
        // first
        std::array<State<T>*, MAX_NUM_NESTED_STATES> exitStateStack;
        State<T>* currentState = this->currentState;
        int stackIndex = 0;
        while (currentState != nullptr) {
            exitStateStack[stackIndex++] = currentState;
            currentState = currentState->parent;
            // Make sure we don't exceed the make number of nested states
            __ASSERT_NO_MSG(stackIndex < MAX_NUM_NESTED_STATES);
        }

        // Call entry functions from top to bottom
        for (int i = stackIndex - 1; i >= 0; i--) {
            exitStateStack[i]->entryFn();
        }

        while (1) {

            // Calculate time to wait for next timeout event
            k_timeout_t timeToWait = K_FOREVER;
            if (this->timer != nullptr && this->timer->isRunning()) {
                int64_t timeToWait_ticks = this->timer->timeToNextFire_ticks - k_uptime_ticks();
                if (timeToWait_ticks < 0) {
                    timeToWait = K_NO_WAIT;
                    printf("Timer already expired\n");
                } else {
                    timeToWait = K_TICKS(timeToWait_ticks);
                    printf("Time to wait in ticks: %lld\n", timeToWait_ticks);
                }
            } else {
                printf("No timer set, waiting forever for message.\n");
            }


            // Block on message queue
            T event;
            int queueRc = k_msgq_get(&msgQueue, &event, timeToWait);

            if (queueRc == 0) {
                printf("Got external event!\n");
                processEvent(event);
            } else {
                printf("Timer must have expired!\n");
                processEvent(this->timer->event);
                // Update timer
                this->timer->incrementNextFireTime();
            }
            
        }
    }

    void processEvent(T event)  {
        // Call the event handler for the current state
        this->currentState->eventFn(event);

        // If the user has requested a state transition, perform it now
        if (this->nextState != nullptr) {
            // We need to find the common parent of the current state and the
            // next state.
            State<T> * commonParentState = this->currentState;
            State<T> * nextState = this->nextState;
            while (commonParentState != nullptr) {
                State<T> * parentState = nextState;
                while (parentState != nullptr) {
                    if (commonParentState == parentState) {
                        break;
                    }
                    parentState = parentState->parent;
                }
                if (parentState != nullptr) {
                    break;
                }
                commonParentState = commonParentState->parent;
            }

            // At this point commonParentState should either point to the
            // common parent, or be nullptr, which means there was no
            // common parent
            printk("Common parent state: %p\n", commonParentState);

            // Now call the exit functions from child to common parent
            State<T> * currentState = this->currentState;
            while (currentState != commonParentState) {
                currentState->exitFn();
                currentState = currentState->parent;
            }

            // Now call the entry functions from common parent to child
            std::array<State<T>*, MAX_NUM_NESTED_STATES> entryStateStack;
            currentState = this->nextState;
            int stackIndex = 0;
            while (currentState != commonParentState) {
                entryStateStack[stackIndex++] = currentState;
                currentState = currentState->parent;
                // Make sure we don't exceed the make number of nested states
                __ASSERT_NO_MSG(stackIndex < MAX_NUM_NESTED_STATES);
            }

            // Call entry functions from top to bottom
            for (int i = stackIndex - 1; i >= 0; i--) {
                entryStateStack[i]->entryFn();
            }

            // Clear out the next state pointer, we've handled it
            this->nextState = nullptr;
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

    Timer<T> * timer;

};