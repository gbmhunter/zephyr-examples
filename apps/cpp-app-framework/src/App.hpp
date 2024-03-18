#pragma once

class App;

#include "StateMachineLibrary/StateMachineController.h"

#include "MasterSm.hpp"
#include "Led.h"

class App
{
    public:
    App();
    void run();

    StateMachineController * getSmc() { return &m_smc; }

    Led * getLed() { return &m_led; }

    private:
    StateMachineController m_smc;

    MasterSm m_masterSm;
    Led m_led;
};