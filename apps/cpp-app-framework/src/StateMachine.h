#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

const uint8_t MAX_NUM_NESTED_STATES = 10;

class Event {
public:
    uint8_t id;
    uint8_t data[10];

    Event() {
        this->id = 0;
        memset(this->data, 0, 10);
    }

    Event(uint8_t id, const char * data) {
        this->id = id;
        if (data != nullptr) {
            memcpy(this->data, data, 10);
        }
    }
};

class Timer {
public:
    int64_t period_ticks;
    int64_t startTime_ticks;
    int64_t timeToNextFire_ticks;
    bool m_isRunning;
    Event event;

    Timer() :
        period_ticks(0),
        startTime_ticks(0),
        timeToNextFire_ticks(0),
        m_isRunning(false)
    {
        // nothing to do
    }

    void start(int64_t period_ticks, Event event) {
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

class State {
public:
    State(
        std::function<void()> entryFn,
        std::function<void(Event)> eventFn,
        std::function<void()> exitFn,
        State * parent = nullptr,
        const char * name = nullptr) :
        entryFn(entryFn),
        eventFn(eventFn),
        exitFn(exitFn),
        parent(parent),
        name(name)
    {
        // nothing to do
    }

    std::function<void()> entryFn;
    std::function<void(Event)> eventFn;
    std::function<void()> exitFn;

    State * parent;
    const char * name;
};

class StateMachine {

public:

    StateMachine(uint8_t maxNumStates, z_thread_stack_element * threadStack, uint32_t threadStackSize_B, void (*threadFnAdapter)(void *, void *, void *)) :
        m_numStates(0),
        m_maxNumStates(maxNumStates),
        m_nextState(nullptr),
        timer(nullptr)
    {
        this->states = static_cast<State**>(k_malloc(this->m_maxNumStates * sizeof(State*)));

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

    void join() {
        k_thread_join(&this->thread, K_FOREVER);
    }

    void addState(State * state) {
        __ASSERT(this->m_numStates < this->m_maxNumStates, "Exceeded max number of states of %u when trying to add state \"\".", this->m_maxNumStates, state->name ? state->name : "unknown");
        this->states[this->m_numStates] = state;
        this->m_numStates++;
    }

    void addTimer(Timer * timer) {
        this->timer = timer;
    }

    void initialTransition(State * state)  {
        this->currentState = state;
    }

    void threadFn()  {
        printf("Thread function for SM started...\n");

        // Perform initial transition. Call the top-most state entry function
        // first
        std::array<State*, MAX_NUM_NESTED_STATES> exitStateStack;
        State* currentState = this->currentState;
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

            k_msleep(1000);

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


            // Block on message queue, providing the correct timeout so that if
            // an event is not received, we handle the next timer timeout at the correct
            // time.
            Event event;
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
            if (m_terminateThread) {
                // This will return from the thread function, which terminates it.
                printf("Returning from thread function...\n");
                return;
            }
            
        }
    }

    void processEvent(Event event)  {
        // Loop through states from current state to root, calling event functions.
        // If at any point an event function requests a transition or to stop propagation,
        // we should break out of the loop.
        State * stateToProcess = this->currentState;
        while (stateToProcess != nullptr) {
            // Reset variables which might get changed when event functions are called
            m_terminateThread = false;
            m_nextState = nullptr;
            m_stopPropagation = false;
            stateToProcess->eventFn(event);
            if (m_terminateThread) {
                printf("Terminate thread requested.\n");
                return;
            }
            if (m_nextState != nullptr) {
                executeTransition(m_nextState);
                return;
            }
            if (m_stopPropagation) {
                // Event function requested to stop propagation,
                // so bail from loop
                return;
            }
            stateToProcess = stateToProcess->parent;
        }
    }

    void executeTransition(State * nextState) {
        // We need to find the common parent of the current state and the
        // next state.
        State * commonParentState = this->currentState;
        // State<T> * m_nextState = this->m_nextState;
        while (commonParentState != nullptr) {
            State * parentState = nextState;
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
        State * currentState = this->currentState;
        while (currentState != commonParentState) {
            currentState->exitFn();
            currentState = currentState->parent;
        }

        // Now call the entry functions from one below common parent (common parent is not exited/entered) to child,
        // first we need to build up a stack
        std::array<State*, MAX_NUM_NESTED_STATES> entryStateStack;
        currentState = nextState;
        int stackIndex = 0;
        while (currentState != commonParentState) {
            entryStateStack[stackIndex++] = currentState;
            currentState = currentState->parent;
            // Make sure we don't exceed the make number of nested states
            __ASSERT_NO_MSG(stackIndex < MAX_NUM_NESTED_STATES);
        }

        // Now we have stack, we can call entry functions in order
        for (int i = stackIndex - 1; i >= 0; i--) {
            entryStateStack[i]->entryFn();
        }

        // Finally, update the current state
        this->currentState = nextState;
    }

    void sendEvent(Event event)  {
        k_msgq_put(&msgQueue, &event, K_NO_WAIT);
    }

    void queueTransition(State * state)  {
        // Save state to transition to
        this->m_nextState = state;
    }

    void stopPropagation()  {
        // Do nothing
        this->m_stopPropagation = true;
    }

    void terminateThreadSm()  {
        this->m_terminateThread = true;
    }

private:
    uint8_t m_numStates;
    uint8_t m_maxNumStates;
    State ** states;
    State * currentState;

    // Variables that can be set in state functions
    State * m_nextState;
    bool m_stopPropagation;
    bool m_terminateThread;

    char * msgQueueBuffer;
    struct k_msgq msgQueue;

    z_thread_stack_element * threadStack;
    struct k_thread thread;

    Timer * timer;

};