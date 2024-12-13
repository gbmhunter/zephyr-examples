#include <cstring>
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "Timer.hpp"
#include "EventLoop.hpp"
LOG_MODULE_REGISTER(Timer, LOG_LEVEL_DBG);

Timer::Timer(std::function<void ()> expiryFn) :
        period_ticks(0),
        startTime_ticks(0),
        nextExpiryTime_ticks(0),
        m_isRunning(false),
        m_expiryFn(expiryFn)
    {
        LOG_DBG("Function called.");

        // Register with event loop
        EventLoop * eventLoop = EventLoop::getInstance();
        eventLoop->registerTimer(this);
    }

void Timer::start(int64_t period_ms) {
    // Convert ms to ticks
    LOG_DBG("period_ms: %d", period_ms);
    start(period_ms, period_ms);
}

void Timer::start(int64_t startDuration_ms, int64_t period_ms)
{
    __ASSERT_NO_MSG(startDuration_ms >= 0); // Start time can be 0, which means the timer will fire immediately. Can't be negative!
    __ASSERT_NO_MSG(period_ms >= -1); // Period can be -1, which means the timer will not repeat
    LOG_DBG("startDuration_ms: %lld. period_ms: %lld", startDuration_ms, period_ms);
    this->startTime_ticks = k_uptime_ticks();
    LOG_DBG("startTime_ticks: %" PRId64, this->startTime_ticks);
    // Use ceil and not floor to guarantee a minimum delay
    LOG_DBG("k_ms_to_ticks_ceil64(startDuration_ms): %" PRId64, k_ms_to_ticks_ceil64(startDuration_ms));
    this->nextExpiryTime_ticks = this->startTime_ticks + k_ms_to_ticks_ceil64(startDuration_ms);
    LOG_DBG("startDuration_ms: %" PRId64 ". period_ms: %" PRId64 ". startTime_ticks: %" PRId64 ". nextExpiryTime_ticks: %" PRId64, startDuration_ms, period_ms, this->startTime_ticks, this->nextExpiryTime_ticks);
    if (period_ms == -1)
    {
        this->period_ticks = -1;
    }
    else
    {
        this->period_ticks = k_ms_to_ticks_ceil64(period_ms);
    }

    this->m_isRunning = true;
}

bool Timer::isRunning() {
    return this->m_isRunning;
}

void Timer::updateAfterExpiry() {
    if (this->period_ticks == -1)
    {
        // Timer was one-shot, so stop it
        this->m_isRunning = false;
    }
    else
    {
        // Update expiry time based on the period
        this->nextExpiryTime_ticks += this->period_ticks;
    }
}

void Timer::expiryFn() {
    LOG_DBG("Function called.");
    printf("Function called.\n");
    if (this->m_expiryFn) {
        this->m_expiryFn();
    } else {
        LOG_WRN("No expiry function set.");
    }
}
