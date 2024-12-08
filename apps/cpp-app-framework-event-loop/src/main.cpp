#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "EventLoop.hpp"

LOG_MODULE_REGISTER(Main, LOG_LEVEL_DBG);

#define EVENT_LOOP_STACK_SIZE 1024

K_THREAD_STACK_DEFINE(event_loop_stack, EVENT_LOOP_STACK_SIZE);

int main(void)
{
    EventLoop eventLoop(event_loop_stack, EVENT_LOOP_STACK_SIZE);
    while (true) {
        k_sleep(K_SECONDS(1));
    }
    LOG_DBG("main() returning...\n");
}

