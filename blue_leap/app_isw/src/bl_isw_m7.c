/****
* File Name    : bl_isw_m7.c
* Version      : 1.0.0
* Description  : M7 core thread implementations for Zephyr ISW
* Creation Date: Dec 2024
****/

/****
 * Includes
 ****/
#include "bl_isw.h"
#include "bl_zephyr_osal_cfg.h"
#include <zephyr/logging/log.h>
#include <zephyr/kernel.h>

LOG_MODULE_REGISTER(bl_isw_m7, LOG_LEVEL_INF);

/****
 * External function declarations
 ****/
/* These functions should be implemented in the application layer */
extern void bl_app_1ms(uint32_t core_id);
extern void bl_app_5ms(uint32_t core_id);
extern void bl_app_10ms(uint32_t core_id);
extern void bl_app_20ms(uint32_t core_id);
extern void bl_app_50ms(uint32_t core_id);
extern void bl_app_100ms(uint32_t core_id);
extern void bl_app_200ms(uint32_t core_id);
extern void bl_app_500ms(uint32_t core_id);
extern void bl_app_1000ms(uint32_t core_id);

/****
 * Static variables
 ****/
static uint32_t m7_task_counter[BL_OS_MAX_NUM_THREADS] = {0};

/****
 * Function implementations
 ****/

/**
 * @brief M7 core initialization
 */
void bl_isw_init_m7(void)
{
    LOG_INF("Initializing M7 ISW");

    /* Initialize OSAL */
    bl_osal_init();

    /* Initialize IPC for M7 */
    bl_osal_ipc_init();

    /* Initialize data structures */
    bl_isw_data_init_m7();

    /* Initialize all task modules */
    bl_isw_t1ms_init_m7();
    bl_isw_t5ms_init_m7();
    bl_isw_t10ms_init_m7();
    bl_isw_t20ms_init_m7();
    bl_isw_t50ms_init_m7();
    bl_isw_t100ms_init_m7();
    bl_isw_t200ms_init_m7();
    bl_isw_t500ms_init_m7();
    bl_isw_t1000ms_init_m7();

    LOG_INF("M7 ISW initialization complete");
}

/**
 * @brief M7 data initialization
 */
void bl_isw_data_init_m7(void)
{
    LOG_INF("Initializing M7 data structures");

    /* Reset task counters */
    for (int i = 0; i < BL_OS_MAX_NUM_THREADS; i++) {
        m7_task_counter[i] = 0;
    }

    /* Initialize system status for M7 */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status = 0;
}

/**
 * @brief 1ms task initialization for M7
 */
void bl_isw_t1ms_init_m7(void)
{
    LOG_DBG("1ms task init M7");
}

/**
 * @brief 1ms cyclic thread for M7
 */
void bl_isw_t1ms_main_m7(void)
{
    m7_task_counter[BL_OS_THREAD_1MS_ID]++;

    /* Call application 1ms function */
    bl_app_1ms(BL_CORE_M7_ID);

    /* Update task status */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status |= BIT(BL_OS_THREAD_1MS_ID);
}

/**
 * @brief 5ms task initialization for M7
 */
void bl_isw_t5ms_init_m7(void)
{
    LOG_DBG("5ms task init M7");
}

/**
 * @brief 5ms cyclic thread for M7
 */
void bl_isw_t5ms_main_m7(void)
{
    m7_task_counter[BL_OS_THREAD_5MS_ID]++;

    /* Call application 5ms function */
    bl_app_5ms(BL_CORE_M7_ID);

    /* Update task status */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status |= BIT(BL_OS_THREAD_5MS_ID);
}

/**
 * @brief 10ms task initialization for M7
 */
void bl_isw_t10ms_init_m7(void)
{
    LOG_DBG("10ms task init M7");
}

/**
 * @brief 10ms cyclic thread for M7
 */
void bl_isw_t10ms_main_m7(void)
{
    m7_task_counter[BL_OS_THREAD_10MS_ID]++;

    /* Call application 10ms function */
    bl_app_10ms(BL_CORE_M7_ID);

    /* Update task status */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status |= BIT(BL_OS_THREAD_10MS_ID);
}

/**
 * @brief 20ms task initialization for M7
 */
void bl_isw_t20ms_init_m7(void)
{
    LOG_DBG("20ms task init M7");
}

/**
 * @brief 20ms cyclic thread for M7
 */
void bl_isw_t20ms_main_m7(void)
{
    m7_task_counter[BL_OS_THREAD_20MS_ID]++;

    /* Call application 20ms function */
    bl_app_20ms(BL_CORE_M7_ID);

    /* Update task status */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status |= BIT(BL_OS_THREAD_20MS_ID);
}

/**
 * @brief 50ms task initialization for M7
 */
void bl_isw_t50ms_init_m7(void)
{
    LOG_DBG("50ms task init M7");
}

/**
 * @brief 50ms cyclic thread for M7
 */
void bl_isw_t50ms_main_m7(void)
{
    m7_task_counter[BL_OS_THREAD_50MS_ID]++;

    /* Call application 50ms function */
    bl_app_50ms(BL_CORE_M7_ID);

    /* Update task status */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status |= BIT(BL_OS_THREAD_50MS_ID);
}

/**
 * @brief 100ms task initialization for M7
 */
void bl_isw_t100ms_init_m7(void)
{
    LOG_DBG("100ms task init M7");
}

/**
 * @brief 100ms cyclic thread for M7
 */
void bl_isw_t100ms_main_m7(void)
{
    m7_task_counter[BL_OS_THREAD_100MS_ID]++;

    /* Call application 100ms function */
    bl_app_100ms(BL_CORE_M7_ID);

    /* Update task status */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status |= BIT(BL_OS_THREAD_100MS_ID);
}

/**
 * @brief 200ms task initialization for M7
 */
void bl_isw_t200ms_init_m7(void)
{
    LOG_DBG("200ms task init M7");
}

/**
 * @brief 200ms cyclic thread for M7
 */
void bl_isw_t200ms_main_m7(void)
{
    m7_task_counter[BL_OS_THREAD_200MS_ID]++;

    /* Call application 200ms function */
    bl_app_200ms(BL_CORE_M7_ID);

    /* Update task status */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status |= BIT(BL_OS_THREAD_200MS_ID);
}

/**
 * @brief 500ms task initialization for M7
 */
void bl_isw_t500ms_init_m7(void)
{
    LOG_DBG("500ms task init M7");
}

/**
 * @brief 500ms cyclic thread for M7
 */
void bl_isw_t500ms_main_m7(void)
{
    m7_task_counter[BL_OS_THREAD_500MS_ID]++;

    /* Call application 500ms function */
    bl_app_500ms(BL_CORE_M7_ID);

    /* Update task status */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status |= BIT(BL_OS_THREAD_500MS_ID);
}

/**
 * @brief 1000ms task initialization for M7
 */
void bl_isw_t1000ms_init_m7(void)
{
    LOG_DBG("1000ms task init M7");
}

/**
 * @brief 1000ms cyclic thread for M7
 */
void bl_isw_t1000ms_main_m7(void)
{
    m7_task_counter[BL_OS_THREAD_1000MS_ID]++;

    /* Call application 1000ms function */
    bl_app_1000ms(BL_CORE_M7_ID);

    /* Update task status */
    bl_system_status_t *status = bl_get_system_status();
    status->m7_task_status |= BIT(BL_OS_THREAD_1000MS_ID);

    /* Log task statistics every second */
    LOG_INF("M7 Tasks - 1ms:%u, 10ms:%u, 50ms:%u, 100ms:%u, 1000ms:%u",
            m7_task_counter[BL_OS_THREAD_1MS_ID],
            m7_task_counter[BL_OS_THREAD_10MS_ID],
            m7_task_counter[BL_OS_THREAD_50MS_ID],
            m7_task_counter[BL_OS_THREAD_100MS_ID],
            m7_task_counter[BL_OS_THREAD_1000MS_ID]);
}