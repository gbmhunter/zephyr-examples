#include <cstdio>

#include <zephyr/logging/log.h>

#include "TestSm.h"

LOG_MODULE_REGISTER(TestSm, LOG_LEVEL_DBG);

const uint8_t MAX_NUM_STATES = 10;

TestSm::TestSm(
    z_thread_stack_element * threadStack,
    void (*threadFnAdapter)(void *, void *, void *)) :
        StateMachine(MAX_NUM_STATES, threadStack, 1024, threadFnAdapter),
        root(
            std::bind(&TestSm::Root_Entry, this),
            std::bind(&TestSm::Root_Event, this, std::placeholders::_1),
            std::bind(&TestSm::Root_Exit, this),
            nullptr, "Root"),
        state1(
            std::bind(&TestSm::State1_Entry, this),
            std::bind(&TestSm::State1_Event, this, std::placeholders::_1),
            std::bind(&TestSm::State1_Exit, this),
            &root, "State1"),
        state2(
            std::bind(&TestSm::State2_Entry, this),
            std::bind(&TestSm::State2_Event, this, std::placeholders::_1),
            std::bind(&TestSm::State2_Exit, this),
            &root, "State2"),
        state2A(
            std::bind(&TestSm::State2A_Entry, this),
            std::bind(&TestSm::State2A_Event, this, std::placeholders::_1),
            std::bind(&TestSm::State2A_Exit, this),
            &state2, "State2A"),
        root2(
            std::bind(&TestSm::Root2_Entry, this),
            std::bind(&TestSm::Root2_Event, this, std::placeholders::_1),
            std::bind(&TestSm::Root2_Exit, this),
            nullptr, "Root2")
    {
    LOG_INF("Test SM created.");


    addState(&root);
    addState(&state1);
    addState(&state2);
    addState(&state2A);
    addState(&root2);
    addTimer(&timer);

    // Setup initial transition
    initialTransition(&state1);
}

void TestSm::fireTestEvent1() {
    // Send event to state machine
    auto event = Event((uint8_t)TestSmEventId::TEST_EVENT_1);
    sendEvent2(&event, sizeof(event));
}

void TestSm::fireRootEvent()
{
    // sendEvent(Event((uint8_t)TestSmEventId::ROOT_EVENT, nullptr));
    auto event = Event((uint8_t)TestSmEventId::ROOT_EVENT);
    sendEvent2(&event, sizeof(event));
}

void TestSm::gotoState2A()
{
    // sendEvent(Event((uint8_t)TestSmEventId::GOTO_STATE_2A, nullptr));
    auto event = Event((uint8_t)TestSmEventId::GOTO_STATE_2A);
    sendEvent2(&event, sizeof(event));
}

void TestSm::gotoStateRoot2()
{
    // sendEvent(Event((uint8_t)TestSmEventId::GOTO_STATE_ROOT2, nullptr));
    auto event = Event((uint8_t)TestSmEventId::GOTO_STATE_ROOT2);
    sendEvent2(&event, sizeof(event));
}

void TestSm::addToCallstack(const char * functionName) {
    callstack[callstackIdx++] = functionName;
}

std::array<const char *, CALL_STACK_DEPTH> TestSm::getCallstackAndClear() {
    // Lock mutex
    k_mutex_lock(&mutex, K_FOREVER);
    // Copy callstack
    std::array<const char *, CALL_STACK_DEPTH> callstackCopy = callstack;
    // Clear callstack and index
    callstack.fill(nullptr);
    callstackIdx = 0;
    // Unlock mutex
    k_mutex_unlock(&mutex);
    return callstackCopy;
}

//============================================================
// STATE: Root
//============================================================

void TestSm::Root_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("Root_Entry");
}

void TestSm::Root_Event(Event * event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);
    addToCallstack("Root_Event");

    // Listen for GOTO_STATE_2A event
    if (event->id == (uint8_t)TestSmEventId::GOTO_STATE_2A) {
        queueTransition(&state2A);
    }

    // Listen for GOTO_STATE_ROOT2 event
    if (event->id == (uint8_t)TestSmEventId::GOTO_STATE_ROOT2) {
        queueTransition(&root2);
    }
}

void TestSm::Root_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("Root_Exit");
}

//============================================================
// STATE: Root/State1
//============================================================

void TestSm::State1_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State1_Entry");
}

void TestSm::State1_Event(Event * event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);
    addToCallstack("State1_Event");
    if (event->id == (uint8_t)TestSmEventId::TEST_EVENT_1) {
        queueTransition(&state2);
    }
}

void TestSm::State1_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State1_Exit");
}

//============================================================
// STATE: Root/State2
//============================================================

void TestSm::State2_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State2_Entry");
}

void TestSm::State2_Event(Event * event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);
    addToCallstack("State2_Event");
}

void TestSm::State2_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State2_Exit");
}

//============================================================
// STATE: Root/State2/State2A
//============================================================

void TestSm::State2A_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State2A_Entry");
}

void TestSm::State2A_Event(Event * event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);
    addToCallstack("State2A_Event");
}

void TestSm::State2A_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State2A_Exit");
}

//============================================================
// STATE: Root2
//============================================================

void TestSm::Root2_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("Root2_Entry");
}

void TestSm::Root2_Event(Event * event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event->id);
    addToCallstack("Root2_Event");
}

void TestSm::Root2_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("Root2_Exit");
}
