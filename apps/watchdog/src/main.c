#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/task_wdt/task_wdt.h>

int main(void) {
    // Initialize, passing NULL so we are not using a hardware watchdog
    // also
    int wdRc = task_wdt_init(NULL);
    __ASSERT_NO_MSG(wdRc == 0);

    // Install a new WDT channel
    int wdtChannelId = task_wdt_add(3000, NULL, NULL);
    __ASSERT_NO_MSG(wdtChannelId == 0);

    uint32_t cycleCount = 0;

    while(1) {
        printf("Feeding watchdog.\n");
        int rc = task_wdt_feed(wdtChannelId); // Regularly feed the watchdog to prevent system reset
        __ASSERT_NO_MSG(rc == 0);
       
        if (cycleCount == 5) {
            printf("Oh oh, bug has got this thread stuck!\n");
            while(1) {
                // Do nothing, just hang here
                k_msleep(1000);
            }
        }

        cycleCount += 1;
        // Sleep for a second before cycling around again
        k_msleep(1000);
    }
    return 0;
}
