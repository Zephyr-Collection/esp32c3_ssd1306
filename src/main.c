#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/wifi_mgmt.h>
#include <zephyr/net/dhcpv4_server.h>
#include "display.h"
#include "wifi_connect.h"
#include "time/time.h"
#include "net_sample_common.h"

/* size of stack area used by each thread */
#define STACKSIZE 2048

/* scheduling priority used by each thread */
#define PRIORITY 7
K_THREAD_STACK_DEFINE(thread_init_wifi_stack_area, STACKSIZE);
static struct k_thread thread_init_wifi_data;

void thread_init_wifi(void *dummy1, void *dummy2, void *dummy3)
{
    int retry_count = 0;
    const int max_retries = 5;
    k_sleep(K_SECONDS(5));

    init_wifi();
	connect_to_wifi();

    wait_for_network();
    k_sleep(K_SECONDS(10));
    for (retry_count = 0; retry_count < max_retries; retry_count++) {
        if (do_sntp(AF_INET) == 0) {
            break;
        }
        printk("SNTP query failed, retrying... (%d/%d)", retry_count + 1, max_retries);
        k_sleep(K_SECONDS(3));
    }
}

int main(void)
{
    k_thread_create(&thread_init_wifi_data, thread_init_wifi_stack_area,
                    K_THREAD_STACK_SIZEOF(thread_init_wifi_stack_area),
                    thread_init_wifi, NULL, NULL, NULL,
                    PRIORITY, 0, K_NO_WAIT);
    display_init();
    while (1) {
        display_task_handler();
        k_sleep(K_MSEC(10)); 
    }
}