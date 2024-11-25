#pragma once

#include <zephyr/kernel.h>

class EventLoop {
public:
    EventLoop(z_thread_stack_element * threadStack, uint32_t threadStackSize_B);
    void run();

private:
    struct k_thread thread;
};
