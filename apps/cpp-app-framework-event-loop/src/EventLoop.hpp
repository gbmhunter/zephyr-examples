#pragma once

#include <functional>

#include <zephyr/kernel.h>

#include "Event.hpp"

class EventLoop {
public:
    EventLoop(z_thread_stack_element * threadStack, uint32_t threadStackSize_B);
    void run();

    void postEvent(EventBase * event, uint8_t size);

    /**
     * Schedule a function to be run in the event loop.
     * 
     * This can be called from any thread or interrupt.
     */
    void scheduleRun(std::function<void()> fn);

private:
    struct k_thread thread;
    char * msgQueueBuffer;
    struct k_msgq msgQueue;

};
