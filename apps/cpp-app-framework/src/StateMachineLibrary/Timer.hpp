#pragma once

//================================================================================================//
// FORWARD DECLARATIONS
//================================================================================================//

class Timer;

//================================================================================================//
// INCLUDES
//================================================================================================//

#include <stdint.h>

#include "Event.hpp"

//================================================================================================//
// CLASS DECLARATION
//================================================================================================//

class Timer {
public:
    int64_t period_ticks;
    int64_t startTime_ticks;
    int64_t nextExpiryTime_ticks;
    bool m_isRunning;
    bool m_beforeFirstExpiry;
    Event event;

    Timer() :
        period_ticks(0),
        startTime_ticks(0),
        nextExpiryTime_ticks(0),
        m_isRunning(false)
    {
        // nothing to do
    }

    /**
     * Start the timer in reoccurring mode. The timer will expire for the first time
     * after period_ms from when this is called, and then period_ms after that.
    */
    void start(int64_t period_ms, Event event);

    /**
     * Start the timer in either one-shot or reoccurring mode.
     * 
     * @param startDuration_ms The time to wait before the first expiry. Must either be 0 (no-wait) or positive.
     * @param period_ms The period of the timer. Set to -1 for a one-shot timer, or 0/positive for a recurring timer.
     * @param event The event to fire when the timer expires.
    */
    void start(int64_t startDuration_ms, int64_t period_ms, Event event);

    /**
     * Check if the timer is running.
     * 
     * @return true if the timer is running, false otherwise.
    */
    bool isRunning();

    /**
     * Designed to be called by the state machine when the timer expires.
     * 
     * This will either:
     * 1. Stop the timer if it is a one-shot timer.
     * 2. Update the next expiry time if it is a recurring timer.
    */
    void updateAfterExpiry();
};