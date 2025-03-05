#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/zbus/zbus.h>

#include "StateMachineLibrary/StateMachineController.hpp"

#include "App.hpp"
#include "MasterSm.hpp"


LOG_MODULE_REGISTER(App, LOG_LEVEL_DBG);

// MASTER SM
const uint32_t MASTER_THREAD_STACK_SIZE_B = 1024;
K_THREAD_STACK_DEFINE(masterThreadStack, MASTER_THREAD_STACK_SIZE_B);
MasterSm * l_masterSm = nullptr;
void masterThreadFnAdapter(void *, void *, void *) {
    l_masterSm->threadFn();
}

// SECOND SM
const uint32_t SECOND_THREAD_STACK_SIZE_B = 1024;
K_THREAD_STACK_DEFINE(secondThreadStack, SECOND_THREAD_STACK_SIZE_B);
SecondSm * l_secondSm = nullptr;
void secondThreadFnAdapter(void *, void *, void *) {
    l_secondSm->threadFn();
}

// LED SM
const uint32_t LED_THREAD_STACK_SIZE_B = 1024;
K_THREAD_STACK_DEFINE(ledThreadStack, LED_THREAD_STACK_SIZE_B);
LedSm * l_led = nullptr;
void ledThreadFnAdapter(void *, void *, void *) {
    l_led->threadFn();
}

App::App(IGpio * gpio)
    :
    m_smc(),
    m_masterSm(masterThreadStack,
                MASTER_THREAD_STACK_SIZE_B,
                &masterThreadFnAdapter,
                this),
    m_secondSm(secondThreadStack,
                SECOND_THREAD_STACK_SIZE_B,
                &secondThreadFnAdapter,
                this),
    m_ledSm(ledThreadStack,
            LED_THREAD_STACK_SIZE_B,
            &ledThreadFnAdapter,
            &m_smc,
            "Led1Sm",
            gpio)
{

    l_masterSm = &m_masterSm;
    l_secondSm = &m_secondSm;
    l_led = &m_ledSm;

    // Register the events

    LOG_INF("App created.");
}

void App::run(void)
{
    // Start all the state machines
    LOG_DBG("Starting all state machines...");
    m_smc.startAll();

    auto timeout = K_MSEC(1000);

    k_msleep(100*1000);
    // led.turnOn();

    // k_msleep(1000);

    // // Make the LED flash
    // led.blink(5, 1000, 1000);


    // const struct zbus_channel *chan;

	// while (!zbus_sub_wait(&bar_sub, &chan, K_FOREVER)) {
	// 	struct acc_msg acc;

	// 	if (&acc_data_chan == chan) {
	// 		zbus_chan_read(&acc_data_chan, &acc, K_MSEC(500));

	// 		LOG_INF("From subscriber -> Acc x=%d, y=%d, z=%d", acc.x, acc.y, acc.z);
	// 	}
	// }
    
    // LOG_DBG("Terminating thread\n");
    // led.terminateThread();
    // LOG_DBG("Joining thread\n");
    // led.join();
    // LOG_DBG("main() returning...\n");
}

