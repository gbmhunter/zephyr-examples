#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <etl/array.h>

#include "Event.hpp"
#include "Timer.hpp"

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

    /**
     * Create a timer.
     * 
     * NOT THREAD SAFE. Must be called from the event loop thread.
     * 
     * @param fn The function to call when the timer expires.
     * @return The timer object.
     */
    void registerTimer(Timer * timer);

    /**
     * Expose a single instance of the event loop.  
     */
    static EventLoop * instance;

    static void createInstance(z_thread_stack_element * threadStack, uint32_t threadStackSize_B) {
        if (instance == nullptr) {
            instance = new EventLoop(threadStack, threadStackSize_B);
        }
    }

    static EventLoop * getInstance() {
        return instance;
    }

private:
    struct k_thread thread;
    char * msgQueueBuffer;
    struct k_msgq msgQueue;

    // etl::array<uint32_t, 10> array;

    Timer * m_timers[10];
    uint32_t m_numTimers = 0;

};
