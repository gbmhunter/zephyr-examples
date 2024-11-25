#include <zephyr/logging/log.h>

#include "EventLoop.hpp"

LOG_MODULE_REGISTER(EventLoop, LOG_LEVEL_DBG);

void threadFnAdapter(void * p1, void * p2, void * p3) {
    EventLoop * eventLoop = static_cast<EventLoop*>(p1);
    eventLoop->run();
}

EventLoop::EventLoop(z_thread_stack_element * threadStack, uint32_t threadStackSize_B) {
    LOG_INF("EventLoop constructor");

    // Create a thread
    k_tid_t my_tid = k_thread_create(&this->thread, threadStack,
                                    threadStackSize_B,
                                    &threadFnAdapter,
                                    this, NULL, NULL,
                                    5, 0,
                                    K_NO_WAIT);
}



void EventLoop::run() {
    LOG_INF("EventLoop run()");
    while (true) {
        k_sleep(K_SECONDS(1));
    }
}
