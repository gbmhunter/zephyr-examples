#include <stdio.h>
#include <functional>

#include <zephyr/kernel.h>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "Led.h"
#include "StateMachine.h"


K_THREAD_STACK_DEFINE(ledThreadStack, 1024);

Led * l_led = nullptr;

void ledThreadFnAdapter(void *, void *, void *) {
    printf("ledThreadFnAdapter\n");
    l_led->sm.threadFn();
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( 0 == 1 );
}

int main(void) {

    // auto led = Led(ledThreadStack, &ledThreadFnAdapter);
    // l_led = &led;
    // led.sm.start();
    // // auto sm = StateMachine(10);

    // // printf("Hello, world!\n");
    // k_msleep(1000);
    // led.turnOn();

    // k_msleep(1000);

    // printf("Terminating thread\n");
    // led.terminateThread();
    // printf("Joining thread\n");
    // led.sm.join();
    // printf("main() returning...\n");

    Catch::Session session; // There must be exactly one instance
 
 
  // writing to session.configData() or session.Config() here 
  // overrides command line args
  // only do this if you know you need to

  int numFailed = session.run();
  
  // numFailed is clamped to 255 as some unices only use the lower 8 bits.
  // This clamping has already been applied, so just return it here
  // You can also do any post run clean-up here
  return numFailed;

    // return 0;
}
