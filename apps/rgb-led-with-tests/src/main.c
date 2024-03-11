#include <stdio.h>

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)

#define MY_GPIO DT_NODELABEL(mygpio)


#if DT_NODE_HAS_STATUS(MY_GPIO, okay)
const struct gpio_dt_spec * const uart_dev = GPIO_DT_SPEC_GET(MY_GPIO, gpios);
#else
#error "Node is disabled"
#endif


/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
// static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main() {
    return 0;
}