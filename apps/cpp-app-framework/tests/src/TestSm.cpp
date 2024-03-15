#include <cstdio>

#include <zephyr/logging/log.h>

#include "TestSm.h"

LOG_MODULE_REGISTER(TestSm, LOG_LEVEL_DBG);

const uint8_t MAX_NUM_STATES = 10;

TestSm::TestSm(
    z_thread_stack_element * threadStack,
    void (*threadFnAdapter)(void *, void *, void *)) :
        sm(MAX_NUM_STATES, threadStack, 1024, threadFnAdapter),
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
            &root, "State2")
    {
    LOG_INF("Test SM created.");


    sm.addState(&root);
    sm.addState(&state1);
    sm.addState(&state2);
    sm.addTimer(&timer);

    // Setup initial transition
    sm.initialTransition(&state1);
}

void TestSm::fireTestEvent1() {
    // Send event to state machine
    sm.sendEvent(Event((uint8_t)TestSmEventId::TEST_EVENT_1, nullptr));
}

void TestSm::fireRootEvent()
{
    sm.sendEvent(Event((uint8_t)TestSmEventId::ROOT_EVENT, nullptr));
}

void TestSm::addToCallstack(const char * functionName) {
    callstack[callstackIdx++] = functionName;
}

std::array<const char *, CALL_STACK_DEPTH> TestSm::getCallstackAndClear() {
    // Lock mutex
    k_mutex_lock(&sm.mutex, K_FOREVER);
    // Copy callstack
    std::array<const char *, CALL_STACK_DEPTH> callstackCopy = callstack;
    // Clear callstack and index
    callstack.fill(nullptr);
    callstackIdx = 0;
    // Unlock mutex
    k_mutex_unlock(&sm.mutex);
    return callstackCopy;
}

//============================================================
// STATE: Root
//============================================================

void TestSm::Root_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("Root/Entry");
}

void TestSm::Root_Event(Event event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event.id);
    addToCallstack("Root/Event");
}

void TestSm::Root_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("Root/Exit");
}

//============================================================
// STATE: Root/State1
//============================================================

void TestSm::State1_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State1/Entry");
}

void TestSm::State1_Event(Event event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event.id);
    addToCallstack("State1/Event");
    if (event.id == (uint8_t)TestSmEventId::TEST_EVENT_1) {
        sm.queueTransition(&state2);
    }
}

void TestSm::State1_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State1/Exit");
}

//============================================================
// STATE: Root/State2
//============================================================

void TestSm::State2_Entry() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State2/Entry");
}

void TestSm::State2_Event(Event event) {
    LOG_INF("%s called. event.id: %u.", __PRETTY_FUNCTION__, event.id);
    addToCallstack("State2/Event");
}

void TestSm::State2_Exit() {
    LOG_INF("%s called", __PRETTY_FUNCTION__);
    addToCallstack("State2/Exit");
}