#pragma once

// FORWARD DECLARATIONS
//==================================================================================================

class StateMachine;
class Event;

// INCLUDES
//==================================================================================================

#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "Constants.hpp"
#include "Event.hpp"
#include "State.hpp"
#include "StateMachineController.hpp"
#include "Timer.hpp"

//================================================================================================//
// EVENTS
//================================================================================================//

class TerminateThreadEvent : public Event {
public:
    TerminateThreadEvent() 
        :
        Event(TypeID::value<TerminateThreadEvent>(), "StateMachine::TerminateThreadEvent")
    {
        // nothing to do
    }
};

//================================================================================================//
// CLASS DECLARATION
//================================================================================================//

class StateMachine {

public:

    struct k_mutex mutex;

    StateMachine(
        uint8_t maxNumStates,
        z_thread_stack_element * threadStack,
        uint32_t threadStackSize_B,
        void (*threadFnAdapter)(void *, void *, void *),
        StateMachineController * smc,
        const char * name,
        uint8_t maxNumTimers = 5);

    /**
     * @brief Add a state to the state machine.
     * State transitions will still work without adding the state to the state machine.
     * 
     * @param state Pointer to the state to add.
     * State must remain in scope for the lifetime of the state machine.
    */
    void addState(State * state);

    void registerTimer(Timer * timer) {
        // Check if there is space for the timer
        __ASSERT_NO_MSG(m_numTimers + 1 <= m_maxNumTimers);
        timers[m_numTimers] = timer;
        m_numTimers++;
    }

    void setInitialTransition(State * state);

    // Start the state machine thread.
    void start();

    void terminateThread();

    // Blocks until the state machine thread has terminated.
    void join();

    //! @brief The function to run in the context of the SM thread.
    //! The only reason this is public is because it needs to be called from a C 
    //! "adapter" function.
    //! This function does not return until the state machine is terminated.
    void threadFn();

    //! @brief Get the current state of the state machine. Will block while the state machine is busy.
    //! THREAD-SAFE
    //! @return Pointer to the current state.
    State * currentState();

    /**
     * @brief Send an event to the state machine.
     * 
     * The event is copied into the state machine's message queue, so the caller can
     * safely deallocate the event after this function returns.
     * 
     * THREAD-SAFE.
     * 
     * @param event Pointer to the event to send.
     * @param size Size of the event in bytes.
    */
    void sendEvent(void * event, uint8_t size);

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

    const char * getName() {
        return this->m_name;
    }

private:

    StateMachineController * m_smc;
    const char * m_name;

    uint8_t m_numStates;
    uint8_t m_maxNumStates;
    State ** states;
    State * m_currentState;

    // Variables that can be set in state functions
    State * m_nextState;
    bool m_stopPropagation;
    bool m_terminateThread;

    char * msgQueueBuffer;
    struct k_msgq msgQueue;

    z_thread_stack_element * threadStack;
    struct k_thread thread;

    Timer ** timers;
    uint8_t m_numTimers;
    uint8_t m_maxNumTimers;


    //! Processes an event that has been received on the message queue.
    void processEvent(Event* event);

    void executeTransition(State * nextState);

};