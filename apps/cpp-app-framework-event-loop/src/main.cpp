#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "Timer.hpp"
#include "Led.hpp"
#include "EventLoop.hpp"

LOG_MODULE_REGISTER(Main, LOG_LEVEL_DBG);

int main(void)
{
    EventLoop::createInstance();
    EventLoop * eventLoop = EventLoop::getInstance();

    // This could be called from an interrupt, to say, process a GPIO changing state
    eventLoop->executeInLoop([]() {
        LOG_INF("Running in event loop!");
    });

    Timer timer([]() {
        LOG_INF("1000ms timer expired!");
    });
    timer.start(1000);

    Timer timer2([]() {
        LOG_INF("2100ms timer expired!");
    });
    timer2.start(2100);

    Led led;
    led.flash(1000);

    // This won't return
    eventLoop->run();
}

