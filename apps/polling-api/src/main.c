#include <stdio.h>

#include <zephyr/kernel.h>

struct k_sem mySem1;
struct k_sem mySem2;

void MyThreadFn(void * v1, void * v2, void * v3) {
    printf("THREAD: Thread started.\n");

    struct k_poll_event events[2];
    k_poll_event_init(&events[0],
                      K_POLL_TYPE_SEM_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY,
                      &mySem1);
    k_poll_event_init(&events[1],
                      K_POLL_TYPE_SEM_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY,
                      &mySem2);
    
    while(1) {
        printk("THREAD: Waiting on k_poll()...\n");
        int rc = k_poll(events, 2, K_FOREVER);
        if (rc != 0)
        {
            // Handle error. If a finite time was provided to k_poll()
            // we would also need to check for -EAGAIN returned, which indicates
            // a timeout
        }

        if (events[0].state == K_POLL_STATE_SEM_AVAILABLE) {
            printk("THREAD: Semaphore 1 available.\n");
            k_sem_take(events[0].sem, K_NO_WAIT); // Careful! If this thread was preempted, we might not actually be able to take the semaphore
        } else if (events[1].state == K_POLL_STATE_SEM_AVAILABLE) {
            printk("THREAD: Semaphore 2 available.\n");
            k_sem_take(events[1].sem, K_NO_WAIT);
        }

        // Because we are going to check again, we need to clear the state!
        events[0].state = K_POLL_STATE_NOT_READY;
        events[1].state = K_POLL_STATE_NOT_READY;
    }
}

K_THREAD_STACK_DEFINE(myStack, 500);
struct k_thread my_thread_data;

int main(void) {
    // printf("Creating timer to expire every 1s...\n");
    
    // Initialize semaphores
    k_sem_init(&mySem1, 0, 1);
    k_sem_init(&mySem2, 0, 1);

    // Create thread
    k_thread_create(&my_thread_data, myStack,
                    K_THREAD_STACK_SIZEOF(myStack),
                    MyThreadFn,
                    NULL, NULL, NULL,
                    5, 0, K_NO_WAIT);

    k_msleep(1000);
    // Give semaphore, this should wake up the thread
    printf("MAIN: Giving semaphore 1...\n");
    k_sem_give(&mySem1);

    k_msleep(1000);
    // Give semaphore, this should wake up the thread
    printf("MAIN: Giving semaphore 2...\n");
    k_sem_give(&mySem2);

    return 0;
}
