/*
 * Copyright (c) 2024, Blue-leap
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <openamp_common.h>

LOG_MODULE_REGISTER(cm7_main, LOG_LEVEL_DBG);

/* Local endpoint */
static struct ipc_ept ep;

/* Semaphore to wait for endpoint binding */
static K_SEM_DEFINE(bound_sem, 0, 1);

/*
 * @brief Endpoint bound callback
 *
 * This function is called when the remote core binds to the endpoint.
 *
 * @param priv Private data
 */
static void ep_bound(void *priv)
{
	LOG_INF("IPC endpoint bound");
	k_sem_give(&bound_sem);
}

/*
 * @brief Message reception callback
 *
 * This function is called when a message is received from the remote core.
 *
 * @param data Received data
 * @param len Length of the received data
 * @param priv Private data
 */
static void ep_recv(const void *data, size_t len, void *priv)
{
	rust_openamp_process_received(data, len);
}

static const struct ipc_ept_cfg ep_cfg = {
	.name = "ep0",
	.cb = {
		.bound = ep_bound,
		.received = ep_recv,
	},
};

int main(void)
{
	const struct device *const ipc0_instance = DEVICE_DT_GET(DT_NODELABEL(ipc0));
	int ret;

	LOG_INF("Starting CM7 application");

	/* Initialize Rust OpenAMP library */
	rust_openamp_init();

	ret = ipc_service_open_instance(ipc0_instance);
	if (ret < 0 && ret != -EALREADY) {
		LOG_ERR("Failed to open IPC instance: %d", ret);
		return ret;
	}

	/* Wait a bit to give the CM4 core time to register its endpoint first */
	k_sleep(K_MSEC(1000));

	ret = ipc_service_register_endpoint(ipc0_instance, &ep, &ep_cfg);
	if (ret < 0) {
		LOG_ERR("Failed to register endpoint: %d", ret);
		return ret;
	}

	/* Wait for the endpoint to be bound */
	k_sem_take(&bound_sem, K_FOREVER);

	while (1) {
		char message[64];
		static uint32_t count = 0;

		/* Format the message to be sent */
		snprintf(message, sizeof(message), "Hello from CM7, count: %u", count++);

		/* Send the message to the remote core */
		rust_openamp_send_message(message);
		ret = ipc_service_send(&ep, message, strlen(message));
		if (ret < 0) {
			LOG_ERR("Failed to send message: %d", ret);
		}

		k_msleep(1000);
	}

	return 0;
}