#include <cstring>

#include <zephyr/kernel.h>

#include "Timer.hpp"

void Timer::start(int64_t period_ms, Event * event, uint8_t eventSize) {
    // Convert ms to ticks
    start(period_ms, period_ms, event, eventSize);
}

void Timer::start(int64_t startDuration_ms, int64_t period_ms, Event * event, uint8_t eventSize)
{
    __ASSERT_NO_MSG(startDuration_ms >= 0); // Start time can be 0, which means the timer will fire immediately. Can't be negative!
    __ASSERT_NO_MSG(period_ms >= -1); // Period can be -1, which means the timer will not repeat
    this->startTime_ticks = k_uptime_ticks();
    // Use ceil and not floor to guarantee a minimum delay
    this->nextExpiryTime_ticks = this->startTime_ticks + k_ms_to_ticks_ceil64(startDuration_ms);
    if (period_ms == -1)
    {
        this->period_ticks = -1;
    }
    else
    {
        this->period_ticks = k_ms_to_ticks_ceil64(period_ms);
    }

    __ASSERT(eventSize <= MAX_MSG_SIZE_BYTES,
             "Event size of %u is bigger than the max. event size of %u.", eventSize, MAX_MSG_SIZE_BYTES);

    // Copy the event in it's raw form (this is to capture child classes of Event)
    memcpy(eventAsBytes, event, eventSize);

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