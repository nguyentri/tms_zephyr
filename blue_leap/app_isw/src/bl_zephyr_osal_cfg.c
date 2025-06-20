/****
* File Name    : bl_zephyr_osal_cfg.c
* Version      : 1.0.0
* Description  : This file contains implementation of Zephyr OS abstraction layer.
* Creation Date: Dec 2024
****/

/****
 * Includes
 ****/
#include "bl_zephyr_osal_cfg.h"
#include "bl_isw.h"
#include <zephyr/logging/log.h>
#include <zephyr/ipc/ipc_service.h>

LOG_MODULE_REGISTER(bl_osal, LOG_LEVEL_INF);

/****
 * Global variables
 ****/
bl_osal_context_t g_osal_context = {0};
bl_system_status_t g_system_status = {0};

/* IPC endpoint for inter-core communication */
static struct ipc_ept bl_ipc_ept;
static K_SEM_DEFINE(bl_ipc_bound_sem, 0, 1);

/* Thread stacks */
K_THREAD_STACK_ARRAY_DEFINE(bl_thread_stacks, BL_OS_MAX_NUM_THREADS, BL_THREAD_STACK_SIZE_LARGE);
static struct k_thread bl_thread_data[BL_OS_MAX_NUM_THREADS];

/****
 * Static function prototypes
 ****/
static void bl_ipc_bound_cb(void *priv);
static void bl_ipc_recv_cb(const void *data, size_t len, void *priv);

/****
 * Static variables
 ****/
static struct ipc_ept_cfg bl_ipc_ept_cfg = {
    .name = "bl_ipc",
    .cb = {
        .bound = bl_ipc_bound_cb,
        .received = bl_ipc_recv_cb,
    },
};

/* Message queue for received IPC messages */
K_MSGQ_DEFINE(bl_ipc_rx_msgq, sizeof(bl_ipc_msg_t), BL_IPC_MSG_QUEUE_SIZE, 4);

/****
 * Function implementations
 ****/

/**
 * @brief IPC bound callback
 */
static void bl_ipc_bound_cb(void *priv)
{
    k_sem_give(&bl_ipc_bound_sem);
    LOG_INF("IPC endpoint bound");
}

/**
 * @brief IPC receive callback
 */
static void bl_ipc_recv_cb(const void *data, size_t len, void *priv)
{
    bl_ipc_msg_t msg;
    
    if (len <= sizeof(bl_ipc_msg_t)) {
        memcpy(&msg, data, len);
        k_msgq_put(&bl_ipc_rx_msgq, &msg, K_NO_WAIT);
    } else {
        LOG_ERR("Received IPC message too large: %zu bytes", len);
    }
}

/**
 * @brief Initialize OSAL
 */
int bl_osal_init(void)
{
    if (g_osal_context.initialized) {
        return 0;
    }
    
    LOG_INF("Initializing OSAL");
    
    /* Initialize thread handles */
    for (int i = 0; i < BL_OS_MAX_NUM_THREADS; i++) {
        g_osal_context.thread_handles[i] = NULL;
    }
    
    g_osal_context.active_threads = 0;
    g_osal_context.initialized = true;
    
    return 0;
}

/**
 * @brief Create a thread
 */
int bl_osal_create_thread(bl_thread_config_t *config, k_thread_entry_t entry)
{
    if (!config || !entry || config->thread_id >= BL_OS_MAX_NUM_THREADS) {
        return -EINVAL;
    }
    
    if (g_osal_context.thread_handles[config->thread_id] != NULL) {
        LOG_WRN("Thread %u already exists", config->thread_id);
        return -EEXIST;
    }
    
    g_osal_context.thread_handles[config->thread_id] = k_thread_create(
        &bl_thread_data[config->thread_id],
        bl_thread_stacks[config->thread_id],
        config->stack_size,
        entry,
        NULL, NULL, NULL,
        config->priority,
        0,
        K_NO_WAIT
    );
    
    if (config->name) {
        k_thread_name_set(g_osal_context.thread_handles[config->thread_id], config->name);
    }
    
    g_osal_context.active_threads++;
    LOG_INF("Created thread %s (ID: %u, Priority: %u)", 
            config->name ? config->name : "unnamed", 
            config->thread_id, 
            config->priority);
    
    return 0;
}

/**
 * @brief Start a thread
 */
int bl_osal_start_thread(uint32_t thread_id)
{
    if (thread_id >= BL_OS_MAX_NUM_THREADS || !g_osal_context.thread_handles[thread_id]) {
        return -EINVAL;
    }
    
    k_thread_start(g_osal_context.thread_handles[thread_id]);
    return 0;
}

/**
 * @brief Stop a thread
 */
int bl_osal_stop_thread(uint32_t thread_id)
{
    if (thread_id >= BL_OS_MAX_NUM_THREADS || !g_osal_context.thread_handles[thread_id]) {
        return -EINVAL;
    }
    
    k_thread_abort(g_osal_context.thread_handles[thread_id]);
    g_osal_context.thread_handles[thread_id] = NULL;
    g_osal_context.active_threads--;
    
    return 0;
}

/**
 * @brief Delay in milliseconds
 */
int bl_osal_delay_ms(uint32_t ms)
{
    k_msleep(ms);
    return 0;
}

/**
 * @brief Get system tick in milliseconds
 */
uint32_t bl_osal_get_tick_ms(void)
{
    return k_uptime_get_32();
}

/**
 * @brief Initialize IPC
 */
int bl_osal_ipc_init(void)
{
    const struct device *ipc_dev;
    int ret;
    
    LOG_INF("Initializing IPC");
    
    /* Get IPC device */
    ipc_dev = DEVICE_DT_GET(DT_NODELABEL(mu1));
    if (!device_is_ready(ipc_dev)) {
        LOG_ERR("IPC device not ready");
        return -ENODEV;
    }
    
    /* Register IPC endpoint */
    ret = ipc_service_register_endpoint(ipc_dev, &bl_ipc_ept, &bl_ipc_ept_cfg);
    if (ret < 0) {
        LOG_ERR("Failed to register IPC endpoint: %d", ret);
        return ret;
    }
    
    /* Wait for endpoint to be bound */
    k_sem_take(&bl_ipc_bound_sem, K_FOREVER);
    
    LOG_INF("IPC initialized successfully");
    return 0;
}

/**
 * @brief Send IPC message
 */
int bl_osal_ipc_send(void *data, size_t len)
{
    if (!data || len == 0 || len > BL_IPC_MSG_MAX_SIZE) {
        return -EINVAL;
    }
    
    return ipc_service_send(&bl_ipc_ept, data, len);
}

/**
 * @brief Receive IPC message
 */
int bl_osal_ipc_recv(void *data, size_t len, k_timeout_t timeout)
{
    bl_ipc_msg_t msg;
    int ret;
    
    if (!data || len == 0) {
        return -EINVAL;
    }
    
    ret = k_msgq_get(&bl_ipc_rx_msgq, &msg, timeout);
    if (ret == 0) {
        size_t copy_len = MIN(len, sizeof(msg));
        memcpy(data, &msg, copy_len);
        return copy_len;
    }
    
    return ret;
}

/**
 * @brief Send inter-core message
 */
int bl_ipc_send_msg(bl_ipc_msg_t *msg)
{
    if (!msg) {
        return -EINVAL;
    }
    
    msg->timestamp = bl_osal_get_tick_ms();
    return bl_osal_ipc_send(msg, sizeof(bl_ipc_msg_t));
}

/**
 * @brief Receive inter-core message
 */
int bl_ipc_recv_msg(bl_ipc_msg_t *msg, k_timeout_t timeout)
{
    if (!msg) {
        return -EINVAL;
    }
    
    return bl_osal_ipc_recv(msg, sizeof(bl_ipc_msg_t), timeout);
}

/**
 * @brief Get system status
 */
bl_system_status_t* bl_get_system_status(void)
{
    return &g_system_status;
}

/**
 * @brief Set system initialized status
 */
void bl_set_system_initialized(bool status)
{
    g_system_status.system_initialized = status;
}

/**
 * @brief Set core sync complete status
 */
void bl_set_core_sync_complete(bool status)
{
    g_system_status.core_sync_complete = status;
}