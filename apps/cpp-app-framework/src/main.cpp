#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "Led.h"
#include "MasterSm.hpp"
#include "StateMachine.h"
#include "StateMachineController.h"

LOG_MODULE_REGISTER(Main, LOG_LEVEL_DBG);

// MASTER SM
const uint32_t MASTER_THREAD_STACK_SIZE_B = 1024;
K_THREAD_STACK_DEFINE(masterThreadStack, MASTER_THREAD_STACK_SIZE_B);
MasterSm * l_masterSm = nullptr;
void masterThreadFnAdapter(void *, void *, void *) {
    l_masterSm->threadFn();
}

// LED SM
const uint32_t LED_THREAD_STACK_SIZE_B = 1024;
K_THREAD_STACK_DEFINE(ledThreadStack, LED_THREAD_STACK_SIZE_B);
Led * l_led = nullptr;
void ledThreadFnAdapter(void *, void *, void *) {
    l_led->threadFn();
}

int main(void)
{
    StateMachineController smc;

    auto masterSm = MasterSm(masterThreadStack,
                             MASTER_THREAD_STACK_SIZE_B,
                             &masterThreadFnAdapter,
                             &smc);
    l_masterSm = &masterSm;

    auto led = Led(ledThreadStack,
                   LED_THREAD_STACK_SIZE_B,
                   &ledThreadFnAdapter,
                   &smc,
                   "Led1Sm");
    l_led = &led;

    // Start all the state machines
    smc.startAll();

    k_msleep(1000);
    led.turnOn();

    k_msleep(1000);

    // Make the LED flash
    led.blink(5, 1000, 1000);


    // const struct zbus_channel *chan;

	// while (!zbus_sub_wait(&bar_sub, &chan, K_FOREVER)) {
	// 	struct acc_msg acc;

	// 	if (&acc_data_chan == chan) {
	// 		zbus_chan_read(&acc_data_chan, &acc, K_MSEC(500));

	// 		LOG_INF("From subscriber -> Acc x=%d, y=%d, z=%d", acc.x, acc.y, acc.z);
	// 	}
	// }
    
    LOG_DBG("Terminating thread\n");
    led.terminateThread();
    LOG_DBG("Joining thread\n");
    led.join();
    LOG_DBG("main() returning...\n");
}

