#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "StateMachine.h"

LOG_MODULE_REGISTER(StateMachine, LOG_LEVEL_DBG);

//===========================================================================//
// PUBLIC METHOD DEFINITIONS
//===========================================================================//

StateMachine::StateMachine(uint8_t maxNumStates, z_thread_stack_element * threadStack, uint32_t threadStackSize_B, void (*threadFnAdapter)(void *, void *, void *)) :
        m_numStates(0),
        m_maxNumStates(maxNumStates),
        states(nullptr),
        currentState(nullptr), // Start off with no current state
        m_nextState(nullptr),
        timer(nullptr)
{
    LOG_DBG("Creating state machine...");
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

void StateMachine::initialTransition(State * state) {
    this->m_nextState = state;
}

void StateMachine::start() {
    LOG_DBG("Starting state machine thread...");
    k_thread_start(&this->thread);
}

void StateMachine::join() {
    LOG_DBG("Blocking until state machine thread terminates...");
    k_thread_join(&this->thread, K_FOREVER);
}

void StateMachine::addState(State * state) {
    __ASSERT(this->m_numStates < this->m_maxNumStates, "Exceeded max number of states of %u when trying to add state \"\".", this->m_maxNumStates, state->name ? state->name : "unknown");
    LOG_DBG("Adding state \"%s\" to state machine...", state->name);
    this->states[this->m_numStates] = state;
    this->m_numStates++;
}

void StateMachine::threadFn()  {
    LOG_DBG("Thread function for SM started...");

    // Perform initial transition from no current state to the initial state as
    // stored in m_nextState.
    LOG_DBG("Test1");
    LOG_DBG("this: %p", this);
    __ASSERT(this->m_nextState != nullptr, "No initial state set for state machine.");
    LOG_DBG("Test3");
    LOG_DBG("Calling entry functions for initial state %s...", this->m_nextState->name);
    executeTransition(this->m_nextState);

    while (1) {

        // k_msleep(1000);

        // Calculate time to wait for next timeout event
        k_timeout_t timeToWait = K_FOREVER;
        if (this->timer != nullptr && this->timer->isRunning()) {
            int64_t timeToWait_ticks = this->timer->timeToNextFire_ticks - k_uptime_ticks();
            if (timeToWait_ticks < 0) {
                timeToWait = K_NO_WAIT;
                LOG_DBG("Timer already expired.");
            } else {
                timeToWait = K_TICKS(timeToWait_ticks);
                LOG_DBG("Time to wait in ticks: %lld.", timeToWait_ticks);
            }
        } else {
            LOG_DBG("No timer set, waiting forever for message.");
        }


        // Block on message queue, providing the correct timeout so that if
        // an event is not received, we handle the next timer timeout at the correct
        // time.
        Event event;
        int queueRc = k_msgq_get(&msgQueue, &event, timeToWait);

        if (queueRc == 0) {
            LOG_DBG("SM thread received external event.");
            processEvent(event);
        } else {
            LOG_DBG("SM timer expired.");
            processEvent(this->timer->event);
            // Update timer
            this->timer->incrementNextFireTime();
        }
        if (m_terminateThread) {
            // This will return from the thread function, which terminates it.
            LOG_DBG("Returning from thread function...");
            return;
        }
        
    }
}

//===========================================================================//
// PRIVATE METHOD DEFINITIONS
//===========================================================================//

void StateMachine::executeTransition(State * nextState) {
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
    LOG_DBG("Found common parent state: %s.", commonParentState ? commonParentState->name : "nullptr");

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