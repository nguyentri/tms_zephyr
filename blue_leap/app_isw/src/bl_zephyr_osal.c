/****
* File Name    : zephyr_osal.c
* Version    : 1.0.0
* Description  : This file contains Zephyr OS abstraction layer implementation.
* Creation Date: Dec 2024
****/

#include "zephyr_osal.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(zephyr_osal);

/* Global thread counter for each core */
volatile uint64_t zephyr_os_thread_counter[CONFIG_MP_MAX_NUM_CPUS][ZEPHYR_OS_MAX_NUM_THREADS];

/****
Function definitions
****/

/**
 * @brief Start the Zephyr OS
 * @param app_mode Application mode
 * @return Standard return type
 */
zephyr_std_return_t zephyr_osal_start_os(zephyr_osal_app_mode_t app_mode)
{
    LOG_INF("Starting Zephyr OS with app mode: %d", app_mode);

    /* Initialize thread counters */
    for (int i = 0; i < CONFIG_MP_MAX_NUM_CPUS; i++) {
        for (int j = 0; j < ZEPHYR_OS_MAX_NUM_THREADS; j++) {
            zephyr_os_thread_counter[i][j] = 0;
        }
    }

    return 0;
}

/**
 * @brief Start OS for specific core
 * @param core_id Core ID
 */
void zephyr_osal_start_os_core(uint32_t core_id)
{
    LOG_INF("Starting OS for core: %d", core_id);
    /* Core-specific OS start implementation */
}

/**
 * @brief Shutdown the Zephyr OS
 * @param error Error status
 * @return Standard return type
 */
zephyr_std_return_t zephyr_osal_shutdown_os(zephyr_status_t error)
{
    LOG_WRN("Shutting down Zephyr OS with error: %d", error);
    return 0;
}

/**
 * @brief Get tick count
 * @param counter_id Counter ID
 * @param value Pointer to store the value
 * @return Status
 */
zephyr_status_t zephyr_osal_get_tick_count(uint8_t counter_id, uint32_t *value)
{
    if (value != NULL) {
        *value = (uint32_t)k_uptime_get_32();
        return 0;
    }
    return -1;
}

/**
 * @brief Get thread period
 * @param thread_id Thread ID
 * @return Thread period in milliseconds
 */
uint32_t zephyr_osal_get_thread_period(uint8_t thread_id)
{
    /* Return period based on thread ID */
    switch (thread_id) {
        case ZEPHYR_OS_THREAD_1MS_ID:     return 1;
        case ZEPHYR_OS_THREAD_5MS_ID:     return 5;
        case ZEPHYR_OS_THREAD_10MS_ID:    return 10;
        case ZEPHYR_OS_THREAD_20MS_ID:    return 20;
        case ZEPHYR_OS_THREAD_50MS_ID:    return 50;
        case ZEPHYR_OS_THREAD_100MS_ID:   return 100;
        case ZEPHYR_OS_THREAD_200MS_ID:   return 200;
        case ZEPHYR_OS_THREAD_500MS_ID:   return 500;
        case ZEPHYR_OS_THREAD_1000MS_ID:  return 1000;
        default:                          return 0;
    }
}

/**
 * @brief ISR handler
 * @param eiic Interrupt ID
 */
void zephyr_osal_isr_handler(unsigned long eiic)
{
    LOG_DBG("ISR handler called for interrupt: %lu", eiic);
    /* Handle interrupt */
}

/**
 * @brief Create a message queue
 * @param queue Pointer to message queue
 * @param queue_length Queue length
 * @param item_size Item size
 */
void zephyr_osal_create_queue(struct k_msgq *queue, uint32_t queue_length, uint32_t item_size)
{
    /* Note: In Zephyr, message queues are typically statically defined */
    LOG_INF("Queue creation requested - length: %d, item_size: %d", queue_length, item_size);
}

/**
 * @brief Send to queue
 * @param queue Message queue
 * @param buffer Buffer to send
 * @param length Buffer length
 * @return Status
 */
zephyr_status_t zephyr_osal_send_queue(struct k_msgq *queue, void *buffer, uint32_t length)
{
    if (queue && buffer) {
        return k_msgq_put(queue, buffer, K_NO_WAIT);
    }
    return -1;
}

/**
 * @brief Receive from queue
 * @param queue Message queue
 * @param buffer Buffer to receive
 * @return Status
 */
zephyr_status_t zephyr_osal_receive_queue(struct k_msgq *queue, void *buffer)
{
    if (queue && buffer) {
        return k_msgq_get(queue, buffer, K_FOREVER);
    }
    return -1;
}