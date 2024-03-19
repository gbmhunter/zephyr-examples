#pragma once

#include <stdint.h>

class TypeID
{
    static size_t counter;

public:
    template<typename T>
    static size_t value()
    {
        static size_t id = counter++;
        return id;
    }
};

class Event {
public:
    uint8_t id;
    const char * m_name;

    Event() {
        this->id = 0;
    }

    Event(uint8_t id, const char * name) {
        this->id = id;
        this->m_name = name;
    }
};