#include <zephyr/ztest.h>

#include "StateMachineLibrary/StateMachine.hpp"

#include "TestSm.h"

ZTEST_SUITE(framework_tests, NULL, NULL, NULL, NULL, NULL);


K_THREAD_STACK_DEFINE(smThreadStack, 2048);

TestSm * l_testSm = nullptr;

void smThreadFnAdapter(void *, void *, void *)
{
    l_testSm->threadFn();
}

void checkCallstack(
    TestSm * sm,
    std::array<const char *, CALL_STACK_DEPTH> &expectedCallStack)
{
    auto actualCallstack = sm->getCallstackAndClear();

    for (uint8_t i = 0; i < CALL_STACK_DEPTH; i++)
    {
        if (expectedCallStack[i] == nullptr)
        {
            // Reached end of call stack. Check the same index in
            // the actual callstack is nullptr also, then return
            zassert_is_null(actualCallstack[i], "Actual callstack has more entries than the expected callstack.");
            break;
        }
        printk("Expected callstack entry: %s\n", expectedCallStack[i]);
        zassert_not_null(
            actualCallstack[i],
            "Actual callstack is missing a call with the name %s.",
            expectedCallStack[i]);

        // If we get here both callstacks are non-null, safe to compare the strings
        zassert_equal(strcmp(actualCallstack[i], expectedCallStack[i]),
                      0,
                      "Expected call of %s was not equal to the actual call of %s at index %u.", expectedCallStack[i],
                      actualCallstack[i],
                      i);
    }
}

void checkCurrentState(TestSm * sm, const char * expectedStateName)
{
    State * currentState = sm->currentState();
    zassert_equal(
        strcmp(currentState->name, expectedStateName),
        0,
        "Current state name \"%s\" did not match the expected state name of \"%s\".",
        currentState->name,
        expectedStateName);
}

// The unit tests should really be broken up into individual tests....but
// I couldn't work out how to destroy the thread stack and re-initialize it
// without causing a seg fault after about the 6th unit test. So I've just
// put all the tests in one big test case.
ZTEST(framework_tests, make_sure_transitions_works)
{
    StateMachineController smc;
    
    // Create test SM
    auto testSm = TestSm(smThreadStack, smThreadFnAdapter, &smc);
    l_testSm = &testSm;

    testSm.start();

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are in state 1
    checkCurrentState(&testSm, "State1");

    // Check the call stack
    std::array<const char *, CALL_STACK_DEPTH> expectedCallStack = {
        "Root_Entry",
        "State1_Entry",
     };
    checkCallstack(&testSm, expectedCallStack);

    // Fire event which will cause transition from state 1 to state 2
    TestEvent1 testEvent1;
    testSm.sendEvent(&testEvent1, sizeof(testEvent1));

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are now in state 2
    checkCurrentState(&testSm, "State2");

    // Check the call stack
    expectedCallStack = {
        "State1_Event",
        "State1_Exit",
        "State2_Entry",
     };
    checkCallstack(&testSm, expectedCallStack);

    // Fire event which is not handled by any states, we should see
    // the state 2 and root state event handlers called
    RootEvent rootEvent;
    testSm.sendEvent(&rootEvent, sizeof(rootEvent));

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are still in state 2
    checkCurrentState(&testSm, "State2");

    // Make sure the state 2 and root state event handlers were called
    expectedCallStack = {
        "State2_Event",
        "Root_Event",
     };
    checkCallstack(&testSm, expectedCallStack);

    // Goto state 2A
    GotoState2AEvent gotoState2AEvent;
    testSm.sendEvent(&gotoState2AEvent, sizeof(gotoState2AEvent));

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are now in state 2A
    checkCurrentState(&testSm, "State2A");

    // Check the call stack
    expectedCallStack = {
        "State2_Event",
        "Root_Event",
        "State2A_Entry",
     };
    checkCallstack(&testSm, expectedCallStack);

    //========================================================================//
    // NO TRANSITION TO SAME STATE
    //========================================================================//

    // Tell SM to goto state 2A again. Since we are already there, no transitions should occur
    testSm.sendEvent(&gotoState2AEvent, sizeof(gotoState2AEvent));

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are still in state 2A
    checkCurrentState(&testSm, "State2A");

    // Check the call stack
    expectedCallStack = {
        "State2A_Event",
        "State2_Event",
        "Root_Event",
     };
    checkCallstack(&testSm, expectedCallStack);

    //========================================================================//
    // TEST STOP PROPAGATION
    //========================================================================//

    StopPropagationEvent stopPropagationEvent;
    testSm.sendEvent(&stopPropagationEvent, sizeof(stopPropagationEvent));

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are still in state 2A
    checkCurrentState(&testSm, "State2A");

    // Check the call stack, should just be 2A because it calls
    // stopPropagation()
    expectedCallStack = {
        "State2A_Event",
     };
    checkCallstack(&testSm, expectedCallStack);

    //========================================================================//
    // SEPARATE ROOT STATE
    //========================================================================//

    // Go to state root2, which is not connected to any other states
    GotoStateRoot2Event gotoStateRoot2Event;
    testSm.sendEvent(&gotoStateRoot2Event, sizeof(gotoStateRoot2Event));

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are now in state root2
    checkCurrentState(&testSm, "Root2");

    // Check the call stack
    expectedCallStack = {
        "State2A_Event",
        "State2_Event",
        "Root_Event",
        "State2A_Exit",
        "State2_Exit",
        "Root_Exit", // We leave the root state
        "Root2_Entry",
     };
    checkCallstack(&testSm, expectedCallStack);

    testSm.terminateThread();
    testSm.join();
}