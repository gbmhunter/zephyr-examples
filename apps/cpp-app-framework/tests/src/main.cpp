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
    std::array<const char *, CALL_STACK_DEPTH> &actualCallStack,
    std::array<const char *, CALL_STACK_DEPTH> &expectedCallStack)
{
    printk("Checking callstack...\n");
    for (uint8_t i = 0; i < CALL_STACK_DEPTH; i++)
    {
        if (expectedCallStack[i] == nullptr)
        {
            // Reached end of call stack. Check the same index in
            // the actual callstack is nullptr also, then return
            zassert_is_null(actualCallStack[i], "Actual callstack has more entries than the expected callstack.");
            break;
        }
        printk("Expected callstack entry: %s\n", expectedCallStack[i]);
        zassert_not_null(
            actualCallStack[i],
            "Actual callstack is missing a call with the name %s.",
            expectedCallStack[i]);

        // If we get here both callstacks are non-null, safe to compare the strings
        zassert_equal(strcmp(actualCallStack[i], expectedCallStack[i]), 0, "Expected call of %s was not equal to the actual call of %s at index %u.", expectedCallStack[i], actualCallStack[i], i);
    }
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

    // Create array of expected call stack
    std::array<const char *, CALL_STACK_DEPTH> expectedCallStack = {
        "Root/Entry",
        "State1/Entry",
     };

    // Check the call stack
    auto callstack = testSm.getCallstackAndClear();
    checkCallstack(callstack, expectedCallStack);

    // Fire event which will cause transition from state 1 to state 2
    testSm.fireTestEvent1();

    // Give the state machine time to run
    k_msleep(1000);

    // Create array of expected call stack
    expectedCallStack = {
        "State1/Event",
        "State1/Exit",
        "State2/Entry",
     };

    // Check the call stack
    callstack = testSm.getCallstackAndClear();
    checkCallstack(callstack, expectedCallStack);

    testSm.terminateThread();
    testSm.join();
}