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
    Event event;

    Timer() :
        period_ticks(0),
        startTime_ticks(0),
        nextExpiryTime_ticks(0),
        m_isRunning(false)
    {
        // nothing to do
    }

    void start(int64_t period_ticks, Event event);

    bool isRunning() {
        return this->m_isRunning;
    }

    void incrementNextFireTime() {
        this->nextExpiryTime_ticks += this->period_ticks;
    }
};