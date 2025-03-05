#include <zephyr/logging/log.h>

#include "GpioMock.hpp"

LOG_MODULE_REGISTER(GpioMock, LOG_LEVEL_DBG);

GpioMock::GpioMock()
{
    m_state = false;
}

void GpioMock::setState(bool state)
{
    m_state = state;
    if (m_state) {
        LOG_INF("Gpio going HIGH.");
    } else {
        LOG_INF("Gpio going LOW.");
    }
}

bool GpioMock::getState()
{
    return m_state;
}
