#include <zephyr/ztest.h>

// #include "RgbLed.h"

#include "Led.h"

ZTEST_SUITE(framework_tests, NULL, NULL, NULL, NULL, NULL);

/* The devicetree node identifier for the "led0" alias. */
// #define LED0_NODE DT_ALIAS(led0)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

K_THREAD_STACK_DEFINE(ledThreadStack, 1024);

Led * l_led = nullptr;

void ledThreadFnAdapter(void *, void *, void *) {
    printf("ledThreadFnAdapter\n");
    l_led->threadFn();
}

/**
 * @brief Test Asserts
 *
 * This test verifies various assert macros provided by ztest.
 *
 */
ZTEST(framework_tests, test_assert)
{
	zassert_true(1, "1 was false");
	zassert_false(0, "0 was true");
	zassert_is_null(NULL, "NULL was not NULL");
	zassert_not_null("foo", "\"foo\" was NULL");
	zassert_equal(1, 1, "1 was not equal to 1");
	zassert_equal_ptr(NULL, NULL, "NULL was not equal to NULL");

	printf("Creating Led object\n");
	auto led = Led(ledThreadStack, &ledThreadFnAdapter);
    l_led = &led;
	printf("Done creating Led object\n");
    led.start();

	k_msleep(1000);

	led.terminateThread();
	led.join();
}