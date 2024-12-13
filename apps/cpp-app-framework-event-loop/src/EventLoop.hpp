#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <etl/array.h>

#include "Event.hpp"

class Timer {
public:
    Timer(std::function<void()> fn, uint32_t ms) : fn(fn), ms(ms) {}
private:
    std::function<void()> fn;
    uint32_t ms;
};

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
    void runInLoop(std::function<void()> fn);

    Timer createTimer(std::function<void()> fn, uint32_t ms);

private:
    struct k_thread thread;
    char * msgQueueBuffer;
    struct k_msgq msgQueue;

    etl::array<uint32_t, 10> array;

};
