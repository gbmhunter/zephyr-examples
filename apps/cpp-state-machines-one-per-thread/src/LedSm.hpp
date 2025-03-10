
#pragma once

#include <cstring>

#include <zephyr/kernel.h>

#include "Gpio/IGpio.hpp"
#include "StateMachineLibrary/StateMachine.hpp"

//================================================================================================//
// EVENTS
//================================================================================================//

#pragma pack(push, 1)
class OnEvent : public Event {
public:
    OnEvent() : Event(EventId::value<OnEvent>(), "LedSm::OnEvent")
    {
        // nothing to do
    }
};

class BlinkEvent : public Event {
public:
    uint8_t numTimes;
    uint32_t onTime_ms;
    uint32_t offTime_ms;

    BlinkEvent(uint8_t numTimes, uint32_t onTime_ms, uint32_t offTime_ms)
        :
        Event(EventId::value<BlinkEvent>(), "LedSm::BlinkEvent"),
        numTimes(numTimes),
        onTime_ms(onTime_ms),
        offTime_ms(offTime_ms)
    {
    }
};

class TimerExpiryEvent : public Event {
public:
    TimerExpiryEvent() : Event(EventId::value<TimerExpiryEvent>(), "LedSm::TimerExpiryEvent")
    {
        // nothing to do
    }
};
#pragma pack(pop)

//================================================================================================//
// STATE MACHINE
//================================================================================================//

class LedSm: public StateMachine {
    public:
        LedSm(
            z_thread_stack_element * threadStack,
            uint32_t threadStackSize_B,
            void (*threadFnAdapter)(void *, void *, void *),
            StateMachineController * smc,
            const char * name,
            IGpio * gpio);

        void turnOn();

        void blink(uint8_t numTimes, uint32_t onTime_ms, uint32_t offTime_ms);


    private:
        IGpio * gpio;
        State root;
        void Root_Entry();
        void Root_Event(Event* event);
        void Root_Exit();

        State off;
        void Off_Entry();
        void Off_Event(Event* event);
        void Off_Exit();

        State on;
        void On_Entry();
        void On_Event(Event* event);
        void On_Exit();

        Timer timer;
};