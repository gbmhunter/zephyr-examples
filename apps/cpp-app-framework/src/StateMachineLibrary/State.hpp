#include <functional>

#include "Event.hpp"

class State {
public:
    State(
        std::function<void()> entryFn,
        std::function<void(Event*)> eventFn,
        std::function<void()> exitFn,
        State * parent = nullptr,
        const char * name = "<unknown>") :
        entryFn(entryFn),
        eventFn(eventFn),
        exitFn(exitFn),
        parent(parent),
        name(name)
    {
        // nothing to do
    }

    std::function<void()> entryFn;
    std::function<void(Event*)> eventFn;
    std::function<void()> exitFn;

    State * parent;
    const char * name;
};