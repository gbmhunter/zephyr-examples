#pragma once

#include "StateMachineLibrary/StateMachineController.h"

#include "MasterSm.hpp"
#include "Led.h"

class App
{
    public:
    App();
    void run();

    private:
    StateMachineController m_smc;

    MasterSm m_masterSm;
    Led m_led;
};