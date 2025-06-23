
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

LOG_MODULE_REGISTER(cm4_main, LOG_LEVEL_INF);

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

    /* Echo back with Rust */
    char response[128];
    snprintf(response, sizeof(response), "CM4 (Rust) Echo: %.*s", (int)len, (char *)data);
    rust_openamp_send_message(response);
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

    LOG_INF("OpenAMP Rust CM4 Application started");

#ifdef CONFIG_OPENAMP_SYSBUILD
    LOG_INF("Running in sysbuild mode");
    LOG_INF("CM7 image size: %d bytes", CM7_IMAGE_SIZE);
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

    LOG_INF("CM4 ready, waiting for messages...");

    while (1) {
        k_sleep(K_SECONDS(1));
    }

    return 0;
}
