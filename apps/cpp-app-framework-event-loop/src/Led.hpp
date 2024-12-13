#pragma once

#include <stdint.h>
#include <stdio.h>

#include "Timer.hpp"

class Led {
public:
    Led();
    void on();
    void off();
    void flash(uint32_t ms);

private:
    Timer m_timer;

    void onTimerExpired();
};
