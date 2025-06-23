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

// Register a logging module for this file (CM4 main)
LOG_MODULE_REGISTER(cm4_main, LOG_LEVEL_INF);

/*
 * Rust FFI function declarations
 * These are implemented in the Rust staticlib and linked into the Zephyr app.
 * - rust_openamp_init_cm4: Initialize Rust-side OpenAMP state
 * - rust_openamp_send_message_cm4: Log and count messages sent from Rust
 * - rust_openamp_process_received_cm4: Process incoming messages in Rust
 */
extern void rust_openamp_init_cm4(void);
extern int rust_openamp_send_message_cm4(const char* message);
extern void rust_openamp_process_received_cm4(const char* data, size_t len);

// IPC endpoint structure and state
static struct ipc_ept ep;
static bool endpoint_bound = false;

// Callback: called when the endpoint is bound (ready for communication)
static void ep_bound(void *priv)
{
    LOG_INF("CM4: Endpoint bound");
    endpoint_bound = true;
}

// Callback: called when a message is received from the remote core
static void ep_recv(const void *data, size_t len, void *priv)
{
    LOG_INF("CM4: Received message: %.*s", (int)len, (char *)data);

    // Forward the received message to Rust for further processing
    rust_openamp_process_received_cm4((const char*)data, len);
}

// IPC endpoint configuration: name and callbacks
static struct ipc_ept_cfg ep_cfg = {
    .name = "ep0", // Endpoint name (must match remote side)
    .cb = {
        .bound = ep_bound,    // Called when endpoint is ready
        .received = ep_recv,  // Called on message reception
    },
};

int main(void)
{
    const struct device *ipc0_instance;
    int ret;

    LOG_INF("OpenAMP Rust CM4 Application started");

#ifdef CONFIG_OPENAMP_SYSBUILD
    LOG_INF("Running in sysbuild mode");
    #ifdef CM7_IMAGE_SIZE
    LOG_INF("CM7 image size: %d bytes", CM7_IMAGE_SIZE);
    #endif
#else
    LOG_INF("Running in single-core mode");
#endif

    // Initialize Rust OpenAMP state (logging, counters, etc.)
    rust_openamp_init_cm4();

    // Get the IPC device instance (must match devicetree label)
    ipc0_instance = DEVICE_DT_GET(DT_NODELABEL(ipc0));
    if (!device_is_ready(ipc0_instance)) {
        LOG_ERR("IPC instance is not ready");
        return -ENODEV;
    }

    // Open the IPC service instance (idempotent)
    ret = ipc_service_open_instance(ipc0_instance);
    if (ret < 0 && ret != -EALREADY) {
        LOG_ERR("Failed to open IPC instance: %d", ret);
        return ret;
    }

    // Register the endpoint for communication
    ret = ipc_service_register_endpoint(ipc0_instance, &ep, &ep_cfg);
    if (ret < 0) {
        LOG_ERR("Failed to register endpoint: %d", ret);
        return ret;
    }

    // Wait for the endpoint to be bound by the remote core
    LOG_INF("Waiting for endpoint to be bound...");
    while (!endpoint_bound) {
        k_sleep(K_MSEC(100));
    }

    LOG_INF("Endpoint bound, starting communication...");
    k_sleep(K_SECONDS(1)); // Optional: allow remote to get ready

    /*
     * Main message loop:
     * - Compose a message string
     * - Log and count it in Rust (rust_openamp_send_message_cm4)
     * - Send it over the IPC endpoint
     * - Wait before sending the next message
     */
    for (int i = 0; i < 10; i++) {
        char msg[64];
        snprintf(msg, sizeof(msg), "Hello from CM4 (Rust) - Message %d", i);

        // Log and count the message in Rust
        ret = rust_openamp_send_message_cm4(msg);
        if (ret < 0) {
            LOG_ERR("Failed to send message via Rust: %d", ret);
        } else {
            // Send the message over the Zephyr IPC service
            ret = ipc_service_send(&ep, msg, strlen(msg));
            if (ret < 0) {
                LOG_ERR("Failed to send IPC message: %d", ret);
            }
        }

        k_sleep(K_SECONDS(2)); // Delay between messages
    }

    LOG_INF("CM4 communication test completed");
    return 0;
}
