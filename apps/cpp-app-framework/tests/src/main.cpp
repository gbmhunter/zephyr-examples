#include <zephyr/ztest.h>

#include "StateMachine.h"

#include "TestSm.h"

ZTEST_SUITE(framework_tests, NULL, NULL, NULL, NULL, NULL);


K_THREAD_STACK_DEFINE(smThreadStack, 2048);

TestSm * l_testSm = nullptr;

void smThreadFnAdapter(void *, void *, void *)
{
    l_testSm->threadFn();
}

enum class TestEventId
{
    TEST = (uint8_t)EventId::MAX_VALUE,
};

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
        zassert_equal(strcmp(actualCallstack[i], expectedCallStack[i]), 0, "Expected call of %s was not equal to the actual call of %s at index %u.", expectedCallStack[i], actualCallstack[i], i);
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

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
ZTEST(framework_tests, make_sure_initial_transition_works)
{
    
    // Create test SM
    auto testSm = TestSm(smThreadStack, smThreadFnAdapter);
    l_testSm = &testSm;

    testSm.start();

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are in state 1
    checkCurrentState(&testSm, "State1");

    // Check the call stack
    std::array<const char *, CALL_STACK_DEPTH> expectedCallStack = {
        "Root/Entry",
        "State1/Entry",
     };
    checkCallstack(&testSm, expectedCallStack);

    // Fire event which will cause transition from state 1 to state 2
    testSm.fireTestEvent1();

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are now in state 2
    checkCurrentState(&testSm, "State2");

    // Check the call stack
    expectedCallStack = {
        "State1/Event",
        "State1/Exit",
        "State2/Entry",
     };
    checkCallstack(&testSm, expectedCallStack);

    // Fire event which is not handled by any states, we should see
    // the state 2 and root state event handlers called
    testSm.fireRootEvent();

    // Give the state machine time to run
    k_msleep(1000);

    // Make sure we are still in state 2
    checkCurrentState(&testSm, "State2");

    // Check the call stack
    expectedCallStack = {
        "State2/Event",
        "Root/Event",
     };
    checkCallstack(&testSm, expectedCallStack);

    testSm.terminateThread();
    testSm.join();
}