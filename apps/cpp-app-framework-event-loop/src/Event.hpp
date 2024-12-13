#pragma once

#include <stdint.h>
#include <stddef.h>
#include <functional>
// class EventId
// {
//     static size_t counter;

// public:
//     template<typename T>
//     static size_t value()
//     {
//         static size_t id = counter++;
//         return id;
//     }
// };

enum class EventId {
    RUN_IN_LOOP,
};

class EventBase {
public:
    EventId m_id;
    const char * m_name;

    // Event() {
    //     this->id = 0;
    // }

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
