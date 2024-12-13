#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "Event.hpp"
#include "EventLoop.hpp"
#include "Constants.hpp"
#include "Timer.hpp"
#include "Led.hpp"

LOG_MODULE_REGISTER(EventLoop, LOG_LEVEL_DBG);

EventLoop * EventLoop::instance = nullptr;

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

    Timer timer([]() {
        LOG_INF("1000ms timer expired!");
    });
    timer.start(1000);

    Timer timer2([]() {
        LOG_INF("2100ms timer expired!");
    });
    timer2.start(2100);


    Led led;
    led.flash(1000);

    while (true) {
        // Iterate through all registered timers, and find the one that is expiring next (if any)
        Timer * timerThatIsExpiringNext = nullptr;
        LOG_DBG("Iterating through timers. numTimers: %d", this->m_numTimers);
        for(uint32_t i = 0; i < this->m_numTimers; i++) {
            Timer * timer = this->m_timers[i];
            if (timer->isRunning()) {
                if (timerThatIsExpiringNext == nullptr || timer->nextExpiryTime_ticks < timerThatIsExpiringNext->nextExpiryTime_ticks) {
                    timerThatIsExpiringNext = timer;
                }
            }
        }

        // Convert the expiry time to a duration from now
        // Calculate time to wait for next timeout event
        k_timeout_t durationToWait = K_FOREVER;
        int64_t uptime_ticks = k_uptime_ticks();
        if (timerThatIsExpiringNext != nullptr) {
            LOG_DBG("Timer that is expiring next has nextExpiryTime_ticks: %" PRId64 ". uptime_ticks: %" PRId64, timerThatIsExpiringNext->nextExpiryTime_ticks, uptime_ticks);
            if (timerThatIsExpiringNext->nextExpiryTime_ticks <= uptime_ticks) {
                durationToWait = K_NO_WAIT;
                LOG_WRN("Timer already expired.");
            } else {
                durationToWait = K_TICKS(timerThatIsExpiringNext->nextExpiryTime_ticks - uptime_ticks);
            }
        }

        // Wait for events on message queue
        uint8_t data[MAX_MSG_SIZE_BYTES] = {0};
        int queueRc = k_msgq_get(&msgQueue, data, durationToWait);
        if (queueRc == 0) {
            // Convert to event
            EventBase * event = reinterpret_cast<EventBase*>(data);
            switch (event->m_id) {
                case EventId::RUN_IN_LOOP:
                    RunInLoopEvent * runInLoopEvent = reinterpret_cast<RunInLoopEvent*>(event);
                    runInLoopEvent->m_fn();
                    break;
            }
        }
        else {
            // This must mean that the timer expired!
            
            // This should never be nullptr, as we should have set it to a valid timer before blocking
            // on the message queue
            __ASSERT_NO_MSG(timerThatIsExpiringNext != nullptr);
            timerThatIsExpiringNext->expiryFn();
            // Update timer. This will either stop the timer if it is a one-shot, or update the next expiry time
            timerThatIsExpiringNext->updateAfterExpiry();
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

void EventLoop::registerTimer(Timer * timer) {
    // Add to list of timers
    this->m_timers[this->m_numTimers] = timer;
    this->m_numTimers++;
}
