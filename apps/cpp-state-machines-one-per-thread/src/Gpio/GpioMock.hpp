#pragma once

#include "IGpio.hpp"

class GpioMock : public IGpio {
public:
    GpioMock();
    void setState(bool state) override;
    bool getState() override;

private:
    bool m_state;
};
