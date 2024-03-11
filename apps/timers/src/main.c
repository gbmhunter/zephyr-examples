#include <stdio.h>
#include <zephyr/kernel.h>

extern void MyExpiryFn(struct k_timer * timerId) {
    printf("Timer expired!\n");
}

int main(void) {
    struct k_timer myTimer;
    printf("Creating timer to expire every 1s...\n");
    k_timer_init(&myTimer, MyExpiryFn, NULL);
    k_timer_start(&myTimer, K_MSEC(1000), K_MSEC(1000));

    while (1) {
        k_msleep(1000);
    }
    return 0;
}
