#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

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
        __ASSERT_NO_MSG(this->numStates < this->maxNumStates);
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

        // Perform initial transition
        this->currentState->entryFn();

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
            this->currentState->exitFn();
            this->currentState = this->nextState;
            this->currentState->entryFn();
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