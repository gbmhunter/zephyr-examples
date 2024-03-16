#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "Led.h"
#include "StateMachine.h"

LOG_MODULE_REGISTER(Main, LOG_LEVEL_DBG);

const uint32_t LED_THREAD_STACK_SIZE_B = 1024;

K_THREAD_STACK_DEFINE(ledThreadStack, LED_THREAD_STACK_SIZE_B);

Led * l_led = nullptr;

void ledThreadFnAdapter(void *, void *, void *) {
    l_led->threadFn();
}

struct acc_msg {
	int x;
	int y;
	int z;
};

ZBUS_CHAN_DEFINE(acc_data_chan,  /* Name */
		 struct acc_msg, /* Message type */

		 NULL,                                 /* Validator */
		 NULL,                                 /* User data */
		 ZBUS_OBSERVERS(bar_sub),     /* observers */
		 ZBUS_MSG_INIT(.x = 0, .y = 0, .z = 0) /* Initial value */
);

ZBUS_SUBSCRIBER_DEFINE(bar_sub, 4);

int main(void) {

    auto led = Led(ledThreadStack, LED_THREAD_STACK_SIZE_B, &ledThreadFnAdapter);
    l_led = &led;
    led.start();
    // auto sm = StateMachine(10);

    // printf("Hello, world!\n");
    k_msleep(1000);
    led.turnOn();

    k_msleep(1000);

    // Make the LED flash
    led.blink(5, 1000, 1000);

    const struct zbus_channel *chan;

	while (!zbus_sub_wait(&bar_sub, &chan, K_FOREVER)) {
		struct acc_msg acc;

		if (&acc_data_chan == chan) {
			zbus_chan_read(&acc_data_chan, &acc, K_MSEC(500));

			LOG_INF("From subscriber -> Acc x=%d, y=%d, z=%d", acc.x, acc.y, acc.z);
		}
	}
    
    LOG_DBG("Terminating thread\n");
    led.terminateThread();
    LOG_DBG("Joining thread\n");
    led.join();
    LOG_DBG("main() returning...\n");
}

