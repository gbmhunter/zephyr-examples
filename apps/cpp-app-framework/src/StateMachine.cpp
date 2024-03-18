#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "StateMachine.h"

LOG_MODULE_REGISTER(StateMachine, LOG_LEVEL_DBG);

//===========================================================================//
// PUBLIC METHOD DEFINITIONS
//===========================================================================//

StateMachine::StateMachine(
    uint8_t maxNumStates,
    z_thread_stack_element * threadStack,
    uint32_t threadStackSize_B,
    void (*threadFnAdapter)(void *, void *, void *),
    StateMachineController * smc,
    const char * name) :
        m_smc(smc),
        m_name(name),
        m_numStates(0),
        m_maxNumStates(maxNumStates),
        states(nullptr),
        m_currentState(nullptr), // Start off with no current state
        m_nextState(nullptr),
        timer(nullptr)
{
    LOG_DBG("Creating state machine...");
    this->states = static_cast<State**>(k_malloc(this->m_maxNumStates * sizeof(State*)));

    // Create message queue for receiving messages
    this->msgQueueBuffer = static_cast<char*>(k_malloc(MSG_QUEUE_SIZE * MAX_MSG_SIZE_BYTES));
    k_msgq_init(&msgQueue, msgQueueBuffer, MAX_MSG_SIZE_BYTES, MSG_QUEUE_SIZE);

    // Initialize SM mutex
    int rc = k_mutex_init(&mutex);
    __ASSERT_NO_MSG(rc == 0);

    this->threadStack = threadStack;
    k_tid_t my_tid = k_thread_create(&this->thread, this->threadStack,
                                    threadStackSize_B,
                                    threadFnAdapter,
                                    NULL, NULL, NULL,
                                    5, 0,
                                    K_FOREVER); // Don't start the thread yet

    // Register this state machine with the SMC
    m_smc->registerStateMachine(this);

    printf("StateMachine created\n");
}

void StateMachine::initialTransition(State * state) {
    this->m_nextState = state;
}

void StateMachine::start() {
    LOG_DBG("Starting state machine thread...");
    k_thread_start(&this->thread);
}

void StateMachine::terminateThread() {
    auto event = Event((uint8_t)EventId::TERMINATE_THREAD);
    sendEvent(event);
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

    __ASSERT(this != nullptr, "The state machine \"this\" was equal to nullptr. Did you forget to assign to the static pointer which the adapter function uses?");
    // Lock mutex, this will be unlocked just before we block on the message queue
    k_mutex_lock(&mutex, K_FOREVER);

    // Perform initial transition from no current state to the initial state as
    // stored in m_nextState.
    __ASSERT(this->m_nextState != nullptr,
             "No initial state set for state machine \"%s\".", this->m_name);
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

        // Just about to block on message queue, so unlock SM mutex
        k_mutex_unlock(&mutex);

        // Block on message queue, providing the correct timeout so that if
        // an event is not received, we handle the next timer timeout at the correct
        // time.
        // TODO: Change hardcoded 10 to constant
        char data[MAX_MSG_SIZE_BYTES];
        int queueRc = k_msgq_get(&msgQueue, &data, timeToWait);

        // Lock SM mutex until we get to the top of the loop again
        k_mutex_lock(&mutex, K_FOREVER);

        Event * event = reinterpret_cast<Event*>(&data);

        if (queueRc == 0) {
            LOG_DBG("SM thread received event.");
            if (event->id == (uint8_t)EventId::TERMINATE_THREAD) {
                LOG_DBG("Terminate thread event received. Returning from the thread function...");
                return;
            }
            processEvent(event);
        } else {
            LOG_DBG("SM timer expired.");
            processEvent(&this->timer->event);
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

State * StateMachine::currentState()
{
    // Lock mutex
    k_mutex_lock(&mutex, K_FOREVER);
    // Get current state
    State * currentState = this->m_currentState;
    // Unlock mutex
    k_mutex_unlock(&mutex);
    return currentState;
}

void StateMachine::sendEvent2(void * event, uint8_t size)  {
    // Make sure event is not bigger than message size
    // static_assert(sizeof(T) <= MSG_SIZE_BYTES, "Event size is too big for message queue");
    __ASSERT(size <= MAX_MSG_SIZE_BYTES, "Event size of %u is bigger than the max. event size of %u.", size, MAX_MSG_SIZE_BYTES);

    // Convert to char array
    uint8_t data[MAX_MSG_SIZE_BYTES] = {0};
    memcpy(data, event, size);

    // Log the data we are about to put on the queue
    // Print each byte as hex
    // LOG_DBG("Sending event to SM: ");
    // for (int i = 0; i < MAX_MSG_SIZE_BYTES; i++) {
    //     // data[i] = -2;
    //     LOG_DBG("0x%02X", data[i]);
    // }

    k_msgq_put(&msgQueue, data, K_NO_WAIT);
}

//===========================================================================//
// PRIVATE METHOD DEFINITIONS
//===========================================================================//

void StateMachine::processEvent(Event* event)  {
    // Loop through states from current state to root, calling event functions.
    // If at any point an event function requests a transition or to stop propagation,
    // we should break out of the loop.
    State * stateToProcess = this->m_currentState;
    while (stateToProcess != nullptr) {
        // Reset variables which might get changed when event functions are called
        m_terminateThread = false;
        m_nextState = nullptr;
        m_stopPropagation = false;
        stateToProcess->eventFn(event);
        if (m_terminateThread) {
            LOG_DBG("Terminate thread requested.");
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

void StateMachine::executeTransition(State * nextState) {
    // We need to find the common parent of the current state and the
    // next state.
    State * commonParentState = this->m_currentState;
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
    State * currentState = this->m_currentState;
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
    this->m_currentState = nextState;
}