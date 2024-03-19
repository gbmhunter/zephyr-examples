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

    MasterSm * getMasterSm() { return &m_masterSm; }
    SecondSm * getSecondSm() { return &m_secondSm; }
    LedSm * getLedSm() { return &m_ledSm; }

private:
    StateMachineController m_smc;

    MasterSm m_masterSm;
    SecondSm m_secondSm;
    LedSm m_ledSm;
};