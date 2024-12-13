#pragma once

#include <stdint.h>
#include <stddef.h>
#include <functional>


enum class EventId {
    RUN_IN_LOOP,
};

class EventBase {
public:
    EventId m_id;
    const char * m_name;

    EventBase(EventId id, const char * name) {
        this->m_id = id;
        this->m_name = name;
    }
};

class RunInLoopEvent : public EventBase {
public:
    std::function<void()> m_fn;

    RunInLoopEvent(std::function<void()> fn) : EventBase(EventId::RUN_IN_LOOP, "RunInLoopEvent") {
        this->m_fn = fn;
    }
};
