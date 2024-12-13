#pragma once

#include <stdint.h>
#include <stdio.h>
#include "EventLoop.hpp"

class Led {
public:
    Led() : 
            m_timer([this]() { this->onTimerExpired(); }) {
    }
    void on() {}
    void off() {}
    void flash(uint32_t ms) {
        m_timer.start(ms, ms);
    }

private:
    Timer m_timer;

    void onTimerExpired() {
        printf("Timer expired\n");
    }
};