/****
* File Name    : bl_isw.h
* Version      : 1.0.0
* Description  : This file contains definitions and function declarations for Zephyr application interface.
* Creation Date: Dec 2024
****/
#ifndef bl_isw_H_
#define bl_isw_H_

/****
 * Includes ****/
#include <zephyr/kernel.h>
#include <stdint.h>
#include <stdbool.h>

/****
 * Macro definitions
 ****/

/* Core identification */
#define BL_CORE_M7    0
#define BL_CORE_M4    1

/* Task periods in milliseconds */
#define BL_TASK_1MS_PERIOD     1
#define BL_TASK_5MS_PERIOD     5
#define BL_TASK_10MS_PERIOD    10
#define BL_TASK_20MS_PERIOD    20
#define BL_TASK_50MS_PERIOD    50
#define BL_TASK_100MS_PERIOD   100
#define BL_TASK_200MS_PERIOD   200
#define BL_TASK_500MS_PERIOD   500
#define BL_TASK_1000MS_PERIOD  1000

/****
 * Typedef definitions
 ****/

/* Inter-core message types */
typedef enum {
    BL_MSG_TYPE_SENSOR_DATA = 0,
    BL_MSG_TYPE_FAN_CONTROL,
    BL_MSG_TYPE_CALIBRATION,
    BL_MSG_TYPE_ALARM_STATUS,
    BL_MSG_TYPE_FOTA_TRIGGER,
    BL_MSG_TYPE_SYSTEM_STATUS,
    BL_MSG_TYPE_MAX
} bl_msg_type_t;

/* Inter-core message structure */
typedef struct {
    uint32_t msg_type;
    uint32_t timestamp;
    uint32_t data_len;
    uint8_t  data[256];
} bl_ipc_msg_t;

/* System status structure */
typedef struct {
    bool system_initialized;
    bool core_sync_complete;
    uint32_t m7_task_status;
    uint32_t m4_task_status;
} bl_system_status_t;

/****
 * Global functions
 ****/

/* Core application software initialization functions */
extern void bl_isw_init(void);
extern void bl_isw_init_m7(void);
extern void bl_isw_init_m4(void);

/* Core data initialization functions */
extern void bl_isw_data_init_m7(void);
extern void bl_isw_data_init_m4(void);

/* M7 (Cortex-M7) Initialization functions for corresponding threads */
extern void bl_isw_t1ms_init_m7(void);
extern void bl_isw_t5ms_init_m7(void);
extern void bl_isw_t10ms_init_m7(void);
extern void bl_isw_t20ms_init_m7(void);
extern void bl_isw_t50ms_init_m7(void);
extern void bl_isw_t100ms_init_m7(void);
extern void bl_isw_t200ms_init_m7(void);
extern void bl_isw_t500ms_init_m7(void);
extern void bl_isw_t1000ms_init_m7(void);

/* M7 (Cortex-M7) Main processing functions for corresponding threads */
extern void bl_isw_t1ms_main_m7(void);
extern void bl_isw_t5ms_main_m7(void);
extern void bl_isw_t10ms_main_m7(void);
extern void bl_isw_t20ms_main_m7(void);
extern void bl_isw_t50ms_main_m7(void);
extern void bl_isw_t100ms_main_m7(void);
extern void bl_isw_t200ms_main_m7(void);
extern void bl_isw_t500ms_main_m7(void);
extern void bl_isw_t1000ms_main_m7(void);

/* M4 (Cortex-M4) Initialization functions for corresponding threads */
extern void bl_isw_t1ms_init_m4(void);
extern void bl_isw_t5ms_init_m4(void);
extern void bl_isw_t10ms_init_m4(void);
extern void bl_isw_t20ms_init_m4(void);
extern void bl_isw_t50ms_init_m4(void);
extern void bl_isw_t100ms_init_m4(void);
extern void bl_isw_t200ms_init_m4(void);
extern void bl_isw_t500ms_init_m4(void);
extern void bl_isw_t1000ms_init_m4(void);

/* M4 (Cortex-M4) Main processing functions for corresponding threads */
extern void bl_isw_t1ms_main_m4(void);
extern void bl_isw_t5ms_main_m4(void);
extern void bl_isw_t10ms_main_m4(void);
extern void bl_isw_t20ms_main_m4(void);
extern void bl_isw_t50ms_main_m4(void);
extern void bl_isw_t100ms_main_m4(void);
extern void bl_isw_t200ms_main_m4(void);
extern void bl_isw_t500ms_main_m4(void);
extern void bl_isw_t1000ms_main_m4(void);

/* Inter-core communication functions */
extern int bl_ipc_send_msg(bl_ipc_msg_t *msg);
extern int bl_ipc_recv_msg(bl_ipc_msg_t *msg, k_timeout_t timeout);

/* System status functions */
extern bl_system_status_t* bl_get_system_status(void);
extern void bl_set_system_initialized(bool status);
extern void bl_set_core_sync_complete(bool status);

#endif /* bl_isw_H_ */