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
    while (true) {
        k_sleep(K_SECONDS(1));
    }
}

void EventLoop::postEvent(Event * event, uint8_t size) {
        // Make sure event is not bigger than message size
    // static_assert(sizeof(T) <= MSG_SIZE_BYTES, "Event size is too big for message queue");
    __ASSERT(size <= MAX_MSG_SIZE_BYTES, "Event size of %u is bigger than the max. event size of %u.", size, MAX_MSG_SIZE_BYTES);

    // Convert to char array
    uint8_t data[MAX_MSG_SIZE_BYTES] = {0};
    memcpy(data, event, size);

    // Log the data we are about to put on the queue
    // Print each byte as hex
    // LOG_DBG("Sending event to SM: ");
    // for (int i = 0; i < MAX_MSG_SIZE_BYTES; i++) {
    //     // data[i] = -2;
    //     LOG_DBG("0x%02X", data[i]);
    // }

    k_msgq_put(&msgQueue, data, K_NO_WAIT);
}