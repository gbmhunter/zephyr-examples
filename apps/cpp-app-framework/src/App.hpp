#pragma once

class App;

#include "StateMachineLibrary/StateMachineController.h"

#include "LedSm.hpp"
#include "MasterSm.hpp"
#include "SecondSm.hpp"

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
    SecondSm m_secondSm;
    Led m_led;
};