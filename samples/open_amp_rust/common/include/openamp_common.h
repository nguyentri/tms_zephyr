/*
 * Copyright (c) 2024, Blue-leap
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef OPENAMP_COMMON_H
#define OPENAMP_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/ipc/ipc_service.h>
#include <zephyr/logging/log.h>

/* Rust function prototypes */
extern void rust_openamp_init(void);
extern int rust_openamp_send_message(const char *message);
extern void rust_openamp_process_received(const char *data, size_t len);

#endif /* OPENAMP_COMMON_H */
