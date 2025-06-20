/****
* File Name    : bl_zephyr_osal.h
* Version       : 1.0.0
* Description  : This file contains definitions and function declarations for Zephyr OS abstraction layer.
* Creation Date: 20 Jun 2025
* Author       :  Tri Nguyen
****/
#ifndef ZEPHYR_OSAL_H
#define ZEPHYR_OSAL_H

/****
Includes
****/
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/ipc/ipc_service.h>
#include "zephyr_osal_cfg.h"

/****
Macro definitions
****/
#define ZEPHYR_OSAL_TRUE    (1)
#define ZEPHYR_OSAL_FALSE   (0)

/* Thread priorities - Zephyr cooperative priorities */
#define OSAL_PRIO_IDLE      K_PRIO_COOP(7)     // Lowest priority
#define OSAL_PRIO_LV1       K_PRIO_COOP(5)     // Priority level 1
#define OSAL_PRIO_LV2       K_PRIO_COOP(3)     // Priority level 2
#define OSAL_PRIO_LV3       K_PRIO_COOP(1)     // Highest priority

/* Stack size definitions */
#define OSAL_STACK_MIN      1024               // Minimal stack size
#define OSAL_STACK_MIN_P128  (OSAL_STACK_MIN + 128U)
#define OSAL_STACK_MIN_P256  (OSAL_STACK_MIN + 256U)
#define OSAL_STACK_MIN_P512  (OSAL_STACK_MIN + 512U)
#define OSAL_STACK_MIN_P1024 (OSAL_STACK_MIN + 1024U)
#define OSAL_STACK_MIN_P2048 (OSAL_STACK_MIN + 2048U)
#define OSAL_STACK_MIN_P4096 (OSAL_STACK_MIN + 4096U)

/* Task delay definitions (in milliseconds) */
#define OSAL_CYCLE_TIME_T1    K_MSEC(1)        // 1 ms delay
#define OSAL_CYCLE_TIME_T5    K_MSEC(5)        // 5 ms delay
#define OSAL_CYCLE_TIME_T10   K_MSEC(10)       // 10 ms delay
#define OSAL_CYCLE_TIME_T20   K_MSEC(20)       // 20 ms delay
#define OSAL_CYCLE_TIME_T50   K_MSEC(50)       // 50 ms delay
#define OSAL_CYCLE_TIME_T100  K_MSEC(100)      // 100 ms delay
#define OSAL_CYCLE_TIME_T200  K_MSEC(200)      // 200 ms delay
#define OSAL_CYCLE_TIME_T500  K_MSEC(500)      // 500 ms delay
#define OSAL_CYCLE_TIME_T1000 K_MSEC(1000)     // 1 second delay
#define OSAL_CYCLE_TIME_T2000 K_MSEC(2000)     // 2 seconds delay
#define OSAL_CYCLE_TIME_T5000 K_MSEC(5000)     // 5 seconds delay

/* Mutex timeout */
#define OSAL_MUTEX_BLOCK_TIME K_MSEC(1000)     // 1 second timeout

/* OS App Mode */
#define OSAL_NO_APP_MODE      ((zephyr_osal_app_mode_t)0U)
#define OSAL_DEFAULT_APP_MODE ((zephyr_osal_app_mode_t)1U)

/****
Type definitions
****/
typedef void (*zephyr_func_ptr_t)(void);
typedef uint8_t zephyr_osal_app_mode_t;
typedef int zephyr_std_return_t;
typedef int zephyr_status_t;

typedef enum {
    ZEPHYR_OS_T_SEM_BIN = 0,
    ZEPHYR_OS_T_SEM_COUNT,
    ZEPHYR_OS_T_STATIC_QUEUE,
    ZEPHYR_OS_T_NORMAL_QUEUE,
    ZEPHYR_OS_T_UNUSED_QUEUE
} zephyr_osal_queue_type_t;

typedef struct {
    const zephyr_osal_queue_type_t queue_type;
    const uint32_t queue_length;
    const uint32_t msg_queue_size;
} zephyr_os_msg_queue_type_t;

typedef struct {
    uint32_t irq_id;
    struct k_sem semaphore;
} zephyr_osal_isr_handler_type_t;

typedef struct {
    zephyr_osal_isr_handler_type_t isr_hdl;
    k_tid_t *thread_hdl;
} zephyr_osal_handler_type_t;

/* Thread configuration type */
typedef struct {
    void (*app_init)(void);
    void (*app_main)(void);
    void (*os_thread)(void *, void *, void *);
    const char * const thread_name;
    const uint16_t stack_used;
    void *const params;
    const int thread_prio;
    k_tid_t *thread_handler;
    zephyr_osal_queue_type_t queue_type;
    const k_timeout_t delay_time;
    bool active_thread;
} zephyr_osal_thread_config_t;

/* OS configuration type */
typedef struct {
    const zephyr_osal_thread_config_t *thread_cfg;
} zephyr_osal_config_t;

typedef struct k_mutex zephyr_osal_data_protection_t;

/****
Macros related to interrupts
****/
#define ZEPHYR_ISR_CAT1(x)    void x##_isr(void)
#define ZEPHYR_ISR_CAT2(x)    void zephyr_osal_##x##_cat2_isr(void)

/****
Global variables
****/
extern const zephyr_osal_config_t zephyr_osal_config[CONFIG_MP_MAX_NUM_CPUS];

/****
Global functions
****/
#define zephyr_osal_init_mutex(mutex_ptr)     k_mutex_init(mutex_ptr)
#define zephyr_osal_set_mutex(mutex_ptr)      k_mutex_lock(mutex_ptr, OSAL_MUTEX_BLOCK_TIME)
#define zephyr_osal_release_mutex(mutex_ptr)  k_mutex_unlock(mutex_ptr)

#define zephyr_osal_create_semaphore(sem_ptr) k_sem_init(sem_ptr, 0, 1)
#define zephyr_osal_delay(timeout)            k_sleep(timeout)

/* Thread creation macro */
#define zephyr_osal_create_thread(cfg, stack_area) \
    k_thread_create((cfg)->thread_handler, stack_area, (cfg)->stack_used, \
                   (cfg)->os_thread, NULL, NULL, NULL, (cfg)->thread_prio, 0, K_NO_WAIT)

/****
Export functions
****/
extern zephyr_std_return_t zephyr_osal_start_os(zephyr_osal_app_mode_t app_mode);
extern void zephyr_osal_start_os_core(uint32_t core_id);
extern zephyr_std_return_t zephyr_osal_shutdown_os(zephyr_status_t error);
extern zephyr_status_t zephyr_osal_get_tick_count(uint8_t counter_id, uint32_t *value);
extern uint32_t zephyr_osal_get_thread_period(uint8_t thread_id);
extern void zephyr_osal_isr_handler(unsigned long eiic);
extern void zephyr_osal_create_queue(struct k_msgq *queue, uint32_t queue_length, uint32_t item_size);
extern zephyr_status_t zephyr_osal_send_queue(struct k_msgq *queue, void *buffer, uint32_t length);
extern zephyr_status_t zephyr_osal_receive_queue(struct k_msgq *queue, void *buffer);

/* Interrupt vector tables for M7 and M4 */
extern void (*int_vectors_m7[])(void);
extern void (*int_vectors_m4[])(void);

#define zephyr_os_int_vectors_m7(eic)  ((zephyr_func_ptr_t)(int_vectors_m7[eic]))()
#define zephyr_os_int_vectors_m4(eic)  ((zephyr_func_ptr_t)(int_vectors_m4[eic]))()

#endif /* ZEPHYR_OSAL_H */