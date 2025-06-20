/*
 * Transformer Monitoring Gateway System - M4 Core Application
 * Dual-core implementation for MIMXRT1160 (M7 + M4)
 * Zephyr RTOS Implementation
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/ipc/ipc_service.h>
#include <zephyr/drivers/clock_control.h>
#include <zephyr/drivers/pinctrl.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/pwm.h>

/* Application specific headers */
#include "bl_zephyr_osal_cfg.h"
#include "bl_isw.h"

LOG_MODULE_REGISTER(gateway_m4, LOG_LEVEL_INF);

/* =============================================================================
 * CORE IDENTIFICATION AND CONFIGURATION
 * =============================================================================*/
#ifdef CONFIG_SOC_MIMXRT1166_CM4
    #define CURRENT_CORE "M4"
    #define IS_M4_CORE 1
#else
    #error "This file is for M4 core only"
#endif

/* =============================================================================
 * THREAD STACK SIZES AND PRIORITIES
 * =============================================================================*/

/* High Priority Real-time Tasks */
#define OPENAMP_COMM_M4_STACK_SIZE      2048
#define OPENAMP_COMM_M4_PRIORITY        5

/* Real-time Control Tasks */
#define FAN_CONTROL_STACK_SIZE          2048
#define FAN_CONTROL_PRIORITY            4

/* Real-time Acquisition Tasks */
#define FREQ_BUSHING_ACQ_STACK_SIZE     3072
#define FREQ_BUSHING_ACQ_PRIORITY       6
#define ENV_ACQ_TASK_STACK_SIZE         2048
#define ENV_ACQ_TASK_PRIORITY           7

/* Safety and Alarm Tasks */
#define ALARM_LOCAL_STACK_SIZE          2048
#define ALARM_LOCAL_PRIORITY            3

/* Calibration Tasks */
#define CALIB_EXEC_STACK_SIZE          2048
#define CALIB_EXEC_PRIORITY            8

/* FOTA Tasks */
#define FOTA_TRIGGER_STACK_SIZE        2048
#define FOTA_TRIGGER_PRIORITY          9

/* Task Periods (in milliseconds) */
#define OPENAMP_COMM_PERIOD            10    /* 100Hz */
#define FAN_CONTROL_PERIOD             20    /* 50Hz */
#define FREQ_BUSHING_ACQ_PERIOD        50    /* 20Hz */
#define ENV_ACQ_PERIOD                100    /* 10Hz */
#define ALARM_LOCAL_PERIOD             50    /* 20Hz */
#define CALIB_EXEC_PERIOD             100    /* 10Hz */
#define FOTA_TRIGGER_PERIOD          1000    /* 1Hz */

/* =============================================================================
 * HARDWARE CONFIGURATION
 * =============================================================================*/

/* ADC Configuration */
#define ADC_DEVICE_NAME         DT_LABEL(DT_NODELABEL(adc1))
#define ADC_RESOLUTION          12
#define ADC_GAIN               ADC_GAIN_1
#define ADC_REFERENCE          ADC_REF_INTERNAL
#define ADC_ACQUISITION_TIME   ADC_ACQ_TIME_DEFAULT

/* PWM Configuration for Fan Control */
#define PWM_DEVICE_NAME        DT_LABEL(DT_NODELABEL(pwm1))
#define PWM_CHANNEL            0
#define PWM_PERIOD_NS          20000  /* 50kHz */

/* =============================================================================
 * DATA STRUCTURES
 * =============================================================================*/

/* Sensor data structure */
typedef struct {
    uint32_t timestamp;
    float frequency;
    float bushing_voltage;
    float bushing_current;
    float temperature;
    float humidity;
    float vibration;
} sensor_data_t;

/* Fan control structure */
typedef struct {
    bool enabled;
    uint8_t speed_percent;
    uint8_t mode;  /* 0: Auto, 1: Manual */
    float target_temperature;
} fan_control_t;

/* Alarm status structure */
typedef struct {
    uint32_t active_alarms;
    uint32_t alarm_history;
    uint8_t severity_level;
} alarm_status_t;

/* =============================================================================
 * GLOBAL VARIABLES
 * =============================================================================*/

/* System status flags */
static bool system_initialized = false;
static bool core_sync_complete = false;

/* Sensor data */
static sensor_data_t current_sensor_data = {0};
static fan_control_t fan_control_state = {
    .enabled = true,
    .speed_percent = 50,
    .mode = 0,
    .target_temperature = 25.0
};
static alarm_status_t alarm_status = {0};

/* Task handles */
static struct k_thread openamp_comm_thread;
static struct k_thread fan_control_thread;
static struct k_thread freq_bushing_acq_thread;
static struct k_thread env_acq_thread;
static struct k_thread alarm_local_thread;
static struct k_thread calib_exec_thread;
static struct k_thread fota_trigger_thread;

/* Task stacks */
K_THREAD_STACK_DEFINE(openamp_comm_stack, OPENAMP_COMM_M4_STACK_SIZE);
K_THREAD_STACK_DEFINE(fan_control_stack, FAN_CONTROL_STACK_SIZE);
K_THREAD_STACK_DEFINE(freq_bushing_acq_stack, FREQ_BUSHING_ACQ_STACK_SIZE);
K_THREAD_STACK_DEFINE(env_acq_stack, ENV_ACQ_TASK_STACK_SIZE);
K_THREAD_STACK_DEFINE(alarm_local_stack, ALARM_LOCAL_STACK_SIZE);
K_THREAD_STACK_DEFINE(calib_exec_stack, CALIB_EXEC_STACK_SIZE);
K_THREAD_STACK_DEFINE(fota_trigger_stack, FOTA_TRIGGER_STACK_SIZE);

/* Synchronization */
K_MUTEX_DEFINE(sensor_data_mutex);
K_MUTEX_DEFINE(fan_control_mutex);

/* =============================================================================
 * TASK IMPLEMENTATIONS
 * =============================================================================*/

/**
 * @brief OpenAMP Communication Task (M4)
 * Handles inter-core communication with M7
 */
static void openamp_comm_m4_task(void *p1, void *p2, void *p3)
{
    bl_ipc_msg_t msg;
    int ret;
    uint32_t msg_count = 0;
    
    LOG_INF("OpenAMP M4 task started");
    
    while (1) {
        /* Check for messages from M7 */
        ret = bl_ipc_recv_msg(&msg, K_MSEC(OPENAMP_COMM_PERIOD/2));
        if (ret == 0) {
            LOG_DBG("Received message from M7: type=%d, len=%d", 
                    msg.msg_type, msg.data_len);
            
            /* Process message based on type */
            switch (msg.msg_type) {
                case BL_MSG_TYPE_FAN_CONTROL:
                    /* Update fan control settings */
                    k_mutex_lock(&fan_control_mutex, K_FOREVER);
                    memcpy(&fan_control_state, msg.data, 
                           MIN(msg.data_len, sizeof(fan_control_t)));
                    k_mutex_unlock(&fan_control_mutex);
                    break;
                    
                case BL_MSG_TYPE_CALIBRATION:
                    /* Handle calibration command */
                    LOG_INF("Received calibration command");
                    break;
                    
                case BL_MSG_TYPE_FOTA_TRIGGER:
                    /* Handle FOTA trigger */
                    LOG_INF("Received FOTA trigger");
                    break;
                    
                default:
                    LOG_WRN("Unknown message type: %d", msg.msg_type);
                    break;
            }
        }
        
        /* Send sensor data to M7 periodically */
        if (++msg_count >= 10) {  /* Every 100ms */
            msg_count = 0;
            
            /* Prepare sensor data message */
            msg.msg_type = BL_MSG_TYPE_SENSOR_DATA;
            
            k_mutex_lock(&sensor_data_mutex, K_FOREVER);
            memcpy(msg.data, &current_sensor_data, sizeof(sensor_data_t));
            msg.data_len = sizeof(sensor_data_t);
            k_mutex_unlock(&sensor_data_mutex);
            
            /* Send to M7 */
            ret = bl_ipc_send_msg(&msg);
            if (ret != 0) {
                LOG_ERR("Failed to send sensor data: %d", ret);
            }
        }
        
        k_msleep(OPENAMP_COMM_PERIOD);
    }
}

/**
 * @brief Fan Control Task
 * Executes real-time fan control based on temperature and commands
 */
static void fan_control_task(void *p1, void *p2, void *p3)
{
    const struct device *pwm_dev;
    uint32_t pulse_width;
    
    LOG_INF("Fan control task started");
    
    /* Get PWM device */
    pwm_dev = device_get_binding(PWM_DEVICE_NAME);
    if (!pwm_dev) {
        LOG_ERR("PWM device not found");
        return;
    }
    
    while (1) {
        k_mutex_lock(&fan_control_mutex, K_FOREVER);
        
        if (fan_control_state.enabled) {
            if (fan_control_state.mode == 0) {
                /* Auto mode - adjust based on temperature */
                k_mutex_lock(&sensor_data_mutex, K_FOREVER);
                float temp_diff = current_sensor_data.temperature - 
                                 fan_control_state.target_temperature;
                k_mutex_unlock(&sensor_data_mutex);
                
                /* Simple proportional control */
                if (temp_diff > 0) {
                    fan_control_state.speed_percent = MIN(100, 
                        50 + (uint8_t)(temp_diff * 10));
                } else {
                    fan_control_state.speed_percent = 50;
                }
            }
            
            /* Calculate PWM duty cycle */
            pulse_width = (PWM_PERIOD_NS * fan_control_state.speed_percent) / 100;
            
            /* Set PWM output */
            pwm_set_dt(pwm_dev, PWM_CHANNEL, PWM_PERIOD_NS, pulse_width, 0);
            
            LOG_DBG("Fan speed: %d%%", fan_control_state.speed_percent);
        } else {
            /* Fan disabled - set PWM to 0 */
            pwm_set_dt(pwm_dev, PWM_CHANNEL, PWM_PERIOD_NS, 0, 0);
        }
        
        k_mutex_unlock(&fan_control_mutex);
        
        k_msleep(FAN_CONTROL_PERIOD);
    }
}

/**
 * @brief Frequency/Bushing Acquisition Task
 * High-speed acquisition of electrical parameters
 */
static void freq_bushing_acq_task(void *p1, void *p2, void *p3)
{
    const struct device *adc_dev;
    struct adc_sequence sequence = {0};
    uint16_t buffer[3];  /* For voltage, current, frequency measurements */
    
    LOG_INF("Frequency/Bushing acquisition task started");
    
    /* Get ADC device */
    adc_dev = device_get_binding(ADC_DEVICE_NAME);
    if (!adc_dev) {
        LOG_ERR("ADC device not found");
        return;
    }
    
    /* Configure ADC sequence */
    sequence.buffer = buffer;
    sequence.buffer_size = sizeof(buffer);
    sequence.resolution = ADC_RESOLUTION;
    
    while (1) {
        /* TODO: Implement actual ADC reading and signal processing */
        
        /* Simulate data acquisition */
        k_mutex_lock(&sensor_data_mutex, K_FOREVER);
        current_sensor_data.frequency = 50.0 + ((float)(sys_rand32_get() % 100) - 50) / 100.0;
        current_sensor_data.bushing_voltage = 230.0 + ((float)(sys_rand32_get() % 20) - 10) / 10.0;
        current_sensor_data.bushing_current = 10.0 + ((float)(sys_rand32_get() % 20) - 10) / 10.0;
        current_sensor_data.timestamp = k_uptime_get_32();
        k_mutex_unlock(&sensor_data_mutex);
        
        LOG_DBG("Freq: %.2f Hz, Voltage: %.1f V, Current: %.1f A",
                current_sensor_data.frequency,
                current_sensor_data.bushing_voltage,
                current_sensor_data.bushing_current);
        
        k_msleep(FREQ_BUSHING_ACQ_PERIOD);
    }
}

/**
 * @brief Environmental Acquisition Task
 * Acquires temperature, humidity, and vibration data
 */
static void env_acq_task(void *p1, void *p2, void *p3)
{
    LOG_INF("Environmental acquisition task started");
    
    while (1) {
        /* TODO: Implement actual sensor reading */
        
        /* Simulate environmental data */
        k_mutex_lock(&sensor_data_mutex, K_FOREVER);
        current_sensor_data.temperature = 25.0 + ((float)(sys_rand32_get() % 100) - 50) / 10.0;
        current_sensor_data.humidity = 60.0 + ((float)(sys_rand32_get() % 40) - 20) / 10.0;
        current_sensor_data.vibration = 0.1 + ((float)(sys_rand32_get() % 10)) / 100.0;
        k_mutex_unlock(&sensor_data_mutex);
        
        LOG_DBG("Temp: %.1f°C, Humidity: %.1f%%, Vibration: %.2f g",
                current_sensor_data.temperature,
                current_sensor_data.humidity,
                current_sensor_data.vibration);
        
        k_msleep(ENV_ACQ_PERIOD);
    }
}

/**
 * @brief Local Alarm Task
 * Processes local safety alarms and immediate responses
 */
static void alarm_local_task(void *p1, void *p2, void *p3)
{
    bl_ipc_msg_t msg;
    bool alarm_state_changed = false;
    
    LOG_INF("Local alarm task started");
    
    while (1) {
        alarm_state_changed = false;
        
        /* Check temperature alarm */
        k_mutex_lock(&sensor_data_mutex, K_FOREVER);
        if (current_sensor_data.temperature > 80.0) {
            if (!(alarm_status.active_alarms & BIT(0))) {
                alarm_status.active_alarms |= BIT(0);
                alarm_state_changed = true;
                LOG_WRN("High temperature alarm: %.1f°C", 
                        current_sensor_data.temperature);
            }
        } else {
            if (alarm_status.active_alarms & BIT(0)) {
                alarm_status.active_alarms &= ~BIT(0);
                alarm_state_changed = true;
                LOG_INF("Temperature alarm cleared");
            }
        }
        
        /* Check vibration alarm */
        if (current_sensor_data.vibration > 1.0) {
            if (!(alarm_status.active_alarms & BIT(1))) {
                alarm_status.active_alarms |= BIT(1);
                alarm_state_changed = true;
                LOG_WRN("High vibration alarm: %.2f g", 
                        current_sensor_data.vibration);
            }
        } else {
            if (alarm_status.active_alarms & BIT(1)) {
                alarm_status.active_alarms &= ~BIT(1);
                alarm_state_changed = true;
                LOG_INF("Vibration alarm cleared");
            }
        }
        k_mutex_unlock(&sensor_data_mutex);
        
        /* Send alarm status to M7 if changed */
        if (alarm_state_changed) {
            msg.msg_type = BL_MSG_TYPE_ALARM_STATUS;
            memcpy(msg.data, &alarm_status, sizeof(alarm_status_t));
            msg.data_len = sizeof(alarm_status_t);
            bl_ipc_send_msg(&msg);
        }
        
        /* Update alarm severity */
        if (alarm_status.active_alarms) {
            alarm_status.severity_level = 2;  /* High severity */
        } else {
            alarm_status.severity_level = 0;  /* No alarms */
        }
        
        k_msleep(ALARM_LOCAL_PERIOD);
    }
}

/**
 * @brief Calibration Execution Task
 * Executes calibration routines
 */
static void calib_exec_task(void *p1, void *p2, void *p3)
{
    LOG_INF("Calibration execution task started");
    
    while (1) {
        /* TODO: Implement calibration execution logic */
        LOG_DBG("Calibration execution check");
        
        k_msleep(CALIB_EXEC_PERIOD);
    }
}

/**
 * @brief FOTA Trigger Task
 * Handles firmware update triggers from M7
 */
static void fota_trigger_task(void *p1, void *p2, void *p3)
{
    LOG_INF("FOTA trigger task started");
    
    while (1) {
        /* TODO: Implement FOTA trigger logic */
        LOG_DBG("FOTA trigger check");
        
        k_msleep(FOTA_TRIGGER_PERIOD);
    }
}

/* =============================================================================
 * APPLICATION CALLBACK FUNCTIONS
 * =============================================================================*/

/**
 * @brief Application 1ms callback
 */
void bl_app_1ms(uint32_t core_id)
{
    /* High-frequency processing if needed */
}

/**
 * @brief Application 5ms callback
 */
void bl_app_5ms(uint32_t core_id)
{
    /* 5ms periodic processing */
}

/**
 * @brief Application 10ms callback
 */
void bl_app_10ms(uint32_t core_id)
{
    /* 10ms periodic processing - OpenAMP communication */
}

/**
 * @brief Application 20ms callback
 */
void bl_app_20ms(uint32_t core_id)
{
    /* 20ms periodic processing - Fan control */
}

/**
 * @brief Application 50ms callback
 */
void bl_app_50ms(uint32_t core_id)
{
    /* 50ms periodic processing - Frequency acquisition, Alarms */
}

/**
 * @brief Application 100ms callback
 */
void bl_app_100ms(uint32_t core_id)
{
    /* 100ms periodic processing - Environmental acquisition */
}

/**
 * @brief Application 200ms callback
 */
void bl_app_200ms(uint32_t core_id)
{
    /* 200ms periodic processing */
}

/**
 * @brief Application 500ms callback
 */
void bl_app_500ms(uint32_t core_id)
{
    /* 500ms periodic processing */
}

/**
 * @brief Application 1000ms callback
 */
void bl_app_1000ms(uint32_t core_id)
{
    /* 1s periodic processing */
    static uint32_t seconds = 0;
    seconds++;
    
    LOG_INF("M4 running for %u seconds", seconds);
    LOG_INF("Active alarms: 0x%08x, Severity: %d", 
            alarm_status.active_alarms, alarm_status.severity_level);
}

/* =============================================================================
 * INITIALIZATION FUNCTIONS
 * =============================================================================*/

/**
 * @brief Initialize M4 peripherals
 */
static int init_m4_peripherals(void)
{
    LOG_INF("Initializing M4 peripherals");
    
    /* TODO: Initialize ADCs */
    /* TODO: Initialize PWM for fan control */
    /* TODO: Initialize GPIO for relays */
    /* TODO: Initialize timers */
    /* TODO: Initialize sensors (I2C/SPI) */
    
    return 0;
}

/**
 * @brief Create and start M4 tasks
 */
static int create_m4_tasks(void)
{
    LOG_INF("Creating M4 tasks");
    
    /* Create OpenAMP communication task */
    k_thread_create(&openamp_comm_thread,
                    openamp_comm_stack,
                    K_THREAD_STACK_SIZEOF(openamp_comm_stack),
                    openamp_comm_m4_task,
                    NULL, NULL, NULL,
                    OPENAMP_COMM_M4_PRIORITY,
                    0, K_NO_WAIT);
    k_thread_name_set(&openamp_comm_thread, "openamp_m4");
    
    /* Create Fan control task */
    k_thread_create(&fan_control_thread,
                    fan_control_stack,
                    K_THREAD_STACK_SIZEOF(fan_control_stack),
                    fan_control_task,
                    NULL, NULL, NULL,
                    FAN_CONTROL_PRIORITY,
                    0, K_NO_WAIT);
    k_thread_name_set(&fan_control_thread, "fan_ctrl");
    
    /* Create Frequency/Bushing acquisition task */
    k_thread_create(&freq_bushing_acq_thread,
                    freq_bushing_acq_stack,
                    K_THREAD_STACK_SIZEOF(freq_bushing_acq_stack),
                    freq_bushing_acq_task,
                    NULL, NULL, NULL,
                    FREQ_BUSHING_ACQ_PRIORITY,
                    0, K_NO_WAIT);
    k_thread_name_set(&freq_bushing_acq_thread, "freq_acq");
    
    /* Create Environmental acquisition task */
    k_thread_create(&env_acq_thread,
                    env_acq_stack,
                    K_THREAD_STACK_SIZEOF(env_acq_stack),
                    env_acq_task,
                    NULL, NULL, NULL,
                    ENV_ACQ_TASK_PRIORITY,
                    0, K_NO_WAIT);
    k_thread_name_set(&env_acq_thread, "env_acq");
    
    /* Create Local alarm task */
    k_thread_create(&alarm_local_thread,
                    alarm_local_stack,
                    K_THREAD_STACK_SIZEOF(alarm_local_stack),
                    alarm_local_task,
                    NULL, NULL, NULL,
                    ALARM_LOCAL_PRIORITY,
                    0, K_NO_WAIT);
    k_thread_name_set(&alarm_local_thread, "alarm_local");
    
    /* Create Calibration execution task */
    k_thread_create(&calib_exec_thread,
                    calib_exec_stack,
                    K_THREAD_STACK_SIZEOF(calib_exec_stack),
                    calib_exec_task,
                    NULL, NULL, NULL,
                    CALIB_EXEC_PRIORITY,
                    0, K_NO_WAIT);
    k_thread_name_set(&calib_exec_thread, "calib_exec");
    
    /* Create FOTA trigger task */
    k_thread_create(&fota_trigger_thread,
                    fota_trigger_stack,
                    K_THREAD_STACK_SIZEOF(fota_trigger_stack),
                    fota_trigger_task,
                    NULL, NULL, NULL,
                    FOTA_TRIGGER_PRIORITY,
                    0, K_NO_WAIT);
    k_thread_name_set(&fota_trigger_thread, "fota_trig");
    
    return 0;
}

/**
 * @brief M4 core initialization
 */
static int bl_isw_data_main_m4_init(void)
{
    int ret;
    
    LOG_INF("=== Transformer Gateway M4 Core Initialization ===");
    
    /* Initialize ISW for M4 */
    bl_isw_init_m4();
    
    /* Initialize M4 peripherals */
    ret = init_m4_peripherals();
    if (ret != 0) {
        LOG_ERR("Failed to initialize peripherals: %d", ret);
        return ret;
    }
    
    /* Create M4 specific tasks */
    ret = create_m4_tasks();
    if (ret != 0) {
        LOG_ERR("Failed to create tasks: %d", ret);
        return ret;
    }
    
    /* Set system initialized */
    bl_set_system_initialized(true);
    system_initialized = true;
    
    LOG_INF("M4 initialization complete");
    return 0;
}

/* =============================================================================
 * MAIN FUNCTION
 * =============================================================================*/

int main(void)
{
    LOG_INF("=== Transformer Monitoring Gateway System ===");
    LOG_INF("Core: %s", CURRENT_CORE);
    LOG_INF("Build: " __DATE__ " " __TIME__);
    
    /* Initialize M4 core */
    bl_isw_data_main_m4_init();
    
    /* Main loop */
    while (1) {
        /* Main thread can perform background tasks or sleep */
        k_msleep(1000);
        
        /* Check system health */
        if (!system_initialized) {
            LOG_ERR("System not properly initialized!");
        }
    }
    
    return 0;
}

/* Register system initialization */
SYS_INIT(bl_isw_data_main_m4_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);