#pragma once

class App;

#include "StateMachineLibrary/StateMachineController.hpp"
#include "LedSm.hpp"
#include "MasterSm.hpp"
#include "SecondSm.hpp"
#include "Gpio/IGpio.hpp"

class App
{
public:
    App(IGpio * gpio);
    void run();

    StateMachineController * getSmc() { return &m_smc; }

    MasterSm * getMasterSm() { return &m_masterSm; }
    SecondSm * getSecondSm() { return &m_secondSm; }
    LedSm * getLedSm() { return &m_ledSm; }

private:
    StateMachineController m_smc;

    MasterSm m_masterSm;
    SecondSm m_secondSm;
    LedSm m_ledSm;
};