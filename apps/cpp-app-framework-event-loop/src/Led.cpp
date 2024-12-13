#include <stdint.h>
#include <stdio.h>

#include <zephyr/logging/log.h>

#include "Led.hpp"

LOG_MODULE_REGISTER(Led, LOG_LEVEL_DBG);

Led::Led() : 
        m_timer([this]() { this->onTimerExpired(); }) {
}

void Led::on() {}
void Led::off() {}
void Led::flash(uint32_t ms) {
    m_timer.start(ms, ms);
}

void Led::onTimerExpired() {
    LOG_INF("LED flashing!");
}
