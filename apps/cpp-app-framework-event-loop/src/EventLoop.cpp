#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "Event.hpp"
#include "EventLoop.hpp"
#include "Constants.hpp"

LOG_MODULE_REGISTER(EventLoop, LOG_LEVEL_DBG);

void threadFnAdapter(void * p1, void * p2, void * p3) {
    EventLoop * eventLoop = static_cast<EventLoop*>(p1);
    eventLoop->run();
}

EventLoop::EventLoop(z_thread_stack_element * threadStack, uint32_t threadStackSize_B) {
    LOG_INF("EventLoop constructor");

    // Create message queue for receiving messages
    this->msgQueueBuffer = static_cast<char*>(k_malloc(MSG_QUEUE_SIZE * MAX_MSG_SIZE_BYTES));
    k_msgq_init(&msgQueue, msgQueueBuffer, MAX_MSG_SIZE_BYTES, MSG_QUEUE_SIZE);

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
    int rc = k_thread_name_set(k_current_get(), "EventLoop");
    __ASSERT(rc == 0, "Failed to set thread name. rc: %d", rc);
    while (true) {
        k_sleep(K_SECONDS(1));
        // Wait for events on message queue
        uint8_t data[MAX_MSG_SIZE_BYTES] = {0};
        LOG_INF("Waiting for event...");
        k_msgq_get(&msgQueue, data, K_FOREVER);
        LOG_INF("Event received!");

        // Convert to event
        EventBase * event = reinterpret_cast<EventBase*>(data);
        switch (event->m_id) {
            case EventId::RUN_IN_LOOP:
                // event->m_fn();
                LOG_INF("Calling function...");
                RunInLoopEvent * runInLoopEvent = reinterpret_cast<RunInLoopEvent*>(event);
                runInLoopEvent->m_fn();
                break;
        }
    }
}

void EventLoop::postEvent(EventBase * event, uint8_t size) {
        // Make sure event is not bigger than message size
    // static_assert(sizeof(T) <= MSG_SIZE_BYTES, "Event size is too big for message queue");
    __ASSERT(size <= MAX_MSG_SIZE_BYTES, "Event size of %u is bigger than the max. event size of %u.", size, MAX_MSG_SIZE_BYTES);

    // Convert to char array
    uint8_t data[MAX_MSG_SIZE_BYTES] = {0};
    memcpy(data, event, size);
    k_msgq_put(&msgQueue, data, K_NO_WAIT);
}

void EventLoop::runInLoop(std::function<void()> fn) {
    // Create a run in loop event
    RunInLoopEvent event(fn);
    this->postEvent(&event, sizeof(event));
}
