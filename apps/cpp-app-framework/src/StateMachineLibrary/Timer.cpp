#include <zephyr/kernel.h>

#include "Timer.hpp"

void Timer::start(int64_t period_ticks, Event event) {
    // Start timer
    this->period_ticks = period_ticks;
    this->event = event;

    // Save current time to work out when next to fire
    this->startTime_ticks = k_uptime_ticks();

    this->nextExpiryTime_ticks = this->startTime_ticks + this->period_ticks;

    this->m_isRunning = true;
}