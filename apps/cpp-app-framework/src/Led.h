
#pragma once

#include <cstring>

#include <zephyr/kernel.h>

#include "StateMachine.h"

enum class LedEventId {
    ON = (uint8_t)EventId::MAX_VALUE,
    OFF,
    TIMER_EXPIRED,
    BLINK,
    MAX_VALUE,
};

#pragma pack(push, 1)
class BlinkEvent : public Event {
public:
    uint8_t numTimes;
    uint32_t onTime_ms;
    uint32_t offTime_ms;


    BlinkEvent(uint8_t numTimes, uint32_t onTime_ms, uint32_t offTime_ms)
        :
        Event((uint8_t)LedEventId::BLINK), 
        numTimes(numTimes),
        onTime_ms(onTime_ms),
        offTime_ms(offTime_ms)
    {
    }
};
#pragma pack(pop)

class Led: public StateMachine {
    public:
        Led(z_thread_stack_element * threadStack, uint32_t threadStackSize_B, void (*threadFnAdapter)(void *, void *, void *));

        void turnOn();

        void blink(uint8_t numTimes, uint32_t onTime_ms, uint32_t offTime_ms);

    private:


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