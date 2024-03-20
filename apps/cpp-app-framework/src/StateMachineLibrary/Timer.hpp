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
     * Start a recurring timer with a period and event to fire.
    */
    void start(int64_t period_ms, Event event);

    void start(int64_t startDuration_ms, int64_t period_ms, Event event);

    bool isRunning() {
        return this->m_isRunning;
    }

    void updateAfterExpiry() {
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
};