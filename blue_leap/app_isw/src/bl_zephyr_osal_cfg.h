/****
* File Name    : bl_zephyr_osal_cfg.h
* Version      : 1.0.0
* Description  : This file contains configuration for Zephyr OS layer.
* Creation Date: Dec 2024
****/
#ifndef BL_ZEPHYR_OSAL_CFG_H_
#define BL_ZEPHYR_OSAL_CFG_H_

/****
Includes
****/
#include <zephyr/kernel.h>

/****
Macro definitions
****/
/* Thread ID definitions */
#define BL_OS_THREAD_INIT_ID     0U
#define BL_OS_THREAD_1MS_ID      1U
#define BL_OS_THREAD_5MS_ID      2U
#define BL_OS_THREAD_10MS_ID     3U
#define BL_OS_THREAD_20MS_ID     4U
#define BL_OS_THREAD_50MS_ID     5U
#define BL_OS_THREAD_100MS_ID    6U
#define BL_OS_THREAD_200MS_ID    7U
#define BL_OS_THREAD_500MS_ID    8U
#define BL_OS_THREAD_1000MS_ID   9U
#define BL_OS_MAX_NUM_THREADS    10U

/* Core definitions */
#define BL_CORE_M7_ID            0U
#define BL_CORE_M4_ID            1U

/* Thread stack sizes */
#define BL_THREAD_STACK_SIZE_SMALL   1024
#define BL_THREAD_STACK_SIZE_MEDIUM  2048
#define BL_THREAD_STACK_SIZE_LARGE   4096
#define BL_THREAD_STACK_SIZE_XLARGE  8192

/* Thread priorities (lower number = higher priority) */
#define BL_THREAD_PRIORITY_CRITICAL  1
#define BL_THREAD_PRIORITY_HIGH      5
#define BL_THREAD_PRIORITY_MEDIUM    10
#define BL_THREAD_PRIORITY_LOW       15
#define BL_THREAD_PRIORITY_IDLE      20

/* IPC Configuration */
#define BL_IPC_MSG_QUEUE_SIZE        32
#define BL_IPC_MSG_MAX_SIZE          256

/****
Typedef definitions
****/

/* Thread configuration structure */
typedef struct {
    uint32_t thread_id;
    uint32_t stack_size;
    uint32_t priority;
    uint32_t period_ms;
    const char *name;
} bl_thread_config_t;

/* OSAL initialization structure */
typedef struct {
    bool initialized;
    uint32_t active_threads;
    struct k_thread *thread_handles[BL_OS_MAX_NUM_THREADS];
} bl_osal_context_t;

/****
Global definitions
****/
extern bl_osal_context_t g_osal_context;

/****
Function declarations
****/

/* OSAL initialization */
extern int bl_osal_init(void);

/* Thread management */
extern int bl_osal_create_thread(bl_thread_config_t *config, k_thread_entry_t entry);
extern int bl_osal_start_thread(uint32_t thread_id);
extern int bl_osal_stop_thread(uint32_t thread_id);

/* Synchronization */
extern int bl_osal_delay_ms(uint32_t ms);
extern uint32_t bl_osal_get_tick_ms(void);

/* IPC functions */
extern int bl_osal_ipc_init(void);
extern int bl_osal_ipc_send(void *data, size_t len);
extern int bl_osal_ipc_recv(void *data, size_t len, k_timeout_t timeout);

#endif /* BL_ZEPHYR_OSAL_CFG_H_ */