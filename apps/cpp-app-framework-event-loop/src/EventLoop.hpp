#pragma once

#include <functional>

#include <zephyr/kernel.h>
#include <etl/array.h>

#include "Event.hpp"
#include "Timer.hpp"

class EventLoop {
public:
    
    /**
     * Call to run the event loop. Does not return.
     */
    void run();

    /**
     * Schedule a function to be run in the event loop.
     * 
     * This can be called from any thread or interrupt.
     */
    void executeInLoop(std::function<void()> fn);

    /**
     * Register a timer.
     * 
     * NOT THREAD SAFE. Must be called from the event loop thread.
     * 
     * @param timer The timer to register.
     */
    void registerTimer(Timer * timer);

    static void createInstance() {
        if (m_instance == nullptr) {
            m_instance = new EventLoop();
        }
    }

    static EventLoop * getInstance() {
        return m_instance;
    }

private:
    /**
     * Private constructor. Create instance through createInstance() instead.
     */
    EventLoop();

    void postEvent(EventBase * event, uint8_t size);

    char * msgQueueBuffer;
    struct k_msgq msgQueue;

    // etl::array<uint32_t, 10> array;

    Timer * m_timers[10];
    uint32_t m_numTimers = 0;

    /**
     * Expose a single instance of the event loop.  
     */
    static EventLoop * m_instance;

};
