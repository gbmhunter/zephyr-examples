#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

const uint8_t MAX_NUM_NESTED_STATES = 10;

enum class EventId {
    TERMINATE_THREAD,
    MAX_VALUE,
};

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
        const char * name = "<unknown>") :
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

    struct k_mutex mutex;

    StateMachine(
        uint8_t maxNumStates,
        z_thread_stack_element * threadStack,
        uint32_t threadStackSize_B,
        void (*threadFnAdapter)(void *, void *, void *));

    //! Add a state to the state machine
    //! @param state Pointer to the state to add. 
    //!         State must remain in scope for the lifetime of the state machine.
    void addState(State * state);

    void addTimer(Timer * timer) {
        this->timer = timer;
    }

    void initialTransition(State * state);

    // Start the state machine thread.
    void start();

    void terminateThread();

    // Blocks until the state machine thread has terminated.
    void join();

    //! The function to run in the context of the SM thread.
    //! The only reason this is public is because it needs to be called from a C 
    //! "adapter" function.
    //! This function does not return until the state machine is terminated.
    void threadFn();



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


    //! Processes an event that has been received on the message queue.
    void processEvent(Event event);

    void executeTransition(State * nextState);

};