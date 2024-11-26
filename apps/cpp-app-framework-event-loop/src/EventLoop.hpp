#pragma once

#include <zephyr/kernel.h>

#include "Event.hpp"

class EventLoop {
public:
    EventLoop(z_thread_stack_element * threadStack, uint32_t threadStackSize_B);
    void run();

    void postEvent(Event * event, uint8_t size);

private:
    struct k_thread thread;
    char * msgQueueBuffer;
    struct k_msgq msgQueue;

};
