
/*
 * Copyright (c) 2024 
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include <zephyr/ipc/ipc_service.h>

#ifdef CONFIG_OPENAMP_SYSBUILD
#include <zephyr_image_info.h>
#endif

LOG_MODULE_REGISTER(cm7_main, LOG_LEVEL_INF);

/* Rust function declarations */
extern void rust_openamp_init(void);
extern int rust_openamp_send_message(const char* message);

static struct ipc_ept ep;

static void ep_bound(void *priv)
{
    LOG_INF("Endpoint bound");
}

static void ep_recv(const void *data, size_t len, void *priv)
{
    LOG_INF("Received message: %.*s", (int)len, (char *)data);
}

static struct ipc_ept_cfg ep_cfg = {
    .name = "ep0",
    .cb = {
        .bound = ep_bound,
        .received = ep_recv,
    },
};

int main(void)
{
    const struct device *ipc0_instance;
    int ret;

    LOG_INF("OpenAMP Rust CM7 Application started");

#ifdef CONFIG_OPENAMP_SYSBUILD
    LOG_INF("Running in sysbuild mode");
    LOG_INF("CM4 image size: %d bytes", CM4_IMAGE_SIZE);
#else
    LOG_INF("Running in single-core mode");
#endif

    /* Initialize Rust OpenAMP library */
    rust_openamp_init();

    ipc0_instance = DEVICE_DT_GET(DT_NODELABEL(ipc0));
    if (!device_is_ready(ipc0_instance)) {
        LOG_ERR("IPC instance is not ready");
        return -ENODEV;
    }

    ret = ipc_service_open_instance(ipc0_instance);
    if (ret < 0 && ret != -EALREADY) {
        LOG_ERR("Failed to open IPC instance: %d", ret);
        return ret;
    }

    ret = ipc_service_register_endpoint(ipc0_instance, &ep, &ep_cfg);
    if (ret < 0) {
        LOG_ERR("Failed to register endpoint: %d", ret);
        return ret;
    }

    LOG_INF("Waiting for CM4 to be ready...");
    k_sleep(K_SECONDS(2));

    /* Send messages using Rust */
    for (int i = 0; i < 10; i++) {
        char msg[64];
        snprintf(msg, sizeof(msg), "Hello from CM7 (Rust) - Message %d", i);
        
        ret = rust_openamp_send_message(msg);
        if (ret < 0) {
            LOG_ERR("Failed to send message via Rust: %d", ret);
        }
        
        k_sleep(K_SECONDS(1));
    }

    return 0;
}
