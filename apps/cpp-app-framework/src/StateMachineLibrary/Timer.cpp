#include <zephyr/kernel.h>

#include "Timer.hpp"

void Timer::start(int64_t period_ms, Event event) {
    // Convert ms to ticks
    this->period_ticks = k_ms_to_ticks_floor64(period_ms);
    this->event = event;

    // Save current time to work out when next to fire
    this->startTime_ticks = k_uptime_ticks();

    this->nextExpiryTime_ticks = this->startTime_ticks + this->period_ticks;

    this->m_isRunning = true;
}

void Timer::start(int64_t startDuration_ms, int64_t period_ms, Event event)
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

    this->event = event;
    this->m_isRunning = true;
}