#include <zephyr/ztest.h>

// #include "RgbLed.h"

#include "Led.h"
#include "StateMachine.h"

ZTEST_SUITE(framework_tests, NULL, NULL, NULL, NULL, NULL);


K_THREAD_STACK_DEFINE(ledThreadStack, 2048);

StateMachine * l_sm = nullptr;

void ledThreadFnAdapter(void *, void *, void *) {
    printf("ledThreadFnAdapter\n");
    l_sm->threadFn();
}

enum class TestEventId {
    TEST,
};

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
ZTEST(framework_tests, make_sure_initial_transition_works) {
    std::array<const char *, 3> callStack = { nullptr };
    uint8_t callStackIdx = 0;

    auto sm = StateMachine(10, ledThreadStack, 2048, &ledThreadFnAdapter);
    l_sm = &sm;
    auto root = State(
        [&]() {
            callStack[callStackIdx++] = "Root/Entry";
        },
        [](Event event) {
            printf("Root_Event\n");
        },
        []() {
            printf("Root_Exit\n");
        },
        nullptr, "Root"
    );
    auto state1 = State(
        [&]() {
            printf("State1_Entry\n");
            callStack[callStackIdx++] = "State1/Entry";
        },
        [](Event event) {
            printf("State1_Event\n");
        },
        []() {
            printf("State1_Exit\n");
        },
        &root, "State1"
    );
    sm.addState(&root);
    sm.addState(&state1);

    sm.initialTransition(&state1);

    printf("Calling start\n");
    sm.start();

    // sm.terminateThreadSm();
    // sm.join();
    k_msleep(5000);

    // Create array of expected call stack
    std::array<const char *, 2> expectedCallStack = {
        "Root/Entry",
        "State1/Entry",
    };

    // Iterate over expected call stack and make sure it matches the actual call stack
    for (uint8_t i = 0; i < expectedCallStack.size(); i++) {
        zassert_not_null(callStack[i], "callStack[%u] was NULL", i);
        zassert_equal(strcmp(callStack[i], expectedCallStack[i]), 0, "callStack[%u] was not equal to expectedCallStack[%u]", i, i);
    }

    // Make sure strings are equal
    // zassert_equal(strcmp(callStack[0], "Root/Entry"), 0 , "Root/Entry was not called");
    

}