cmake_minimum_required(VERSION 3.20.0)
# Copyright (c) 2018 Nordic Semiconductor ASA
# Copyright (c) 2019 Linaro Limited
# Copyright 2022 NXP
#
# SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(cm4)

# Add Rust support
find_program(CARGO cargo REQUIRED)
message(STATUS "${BOARD} compiles as the Remote in this sample")

# Add source files
target_sources(app PRIVATE src/main.c)
# Add include directories
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../common)

# Add rust library
add_dependencies(app rust_lib_cm4)

# Rust library configuration
set(RUST_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rust_lib)
set(RUST_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/rust_target)
set(RUST_TARGET thumbv7m-none-eabi)
#set(RUST_TARGET thumbv7em-none-eabihf)

# Build Rust library
add_custom_command(
    OUTPUT ${RUST_TARGET_DIR}/${RUST_TARGET}/release/librust_openamp_cm4.a
    COMMAND ${CMAKE_COMMAND} -E env
        CARGO_TARGET_DIR=${RUST_TARGET_DIR}
        ${CARGO} build --release --target ${RUST_TARGET}
    WORKING_DIRECTORY ${RUST_LIB_DIR}
    DEPENDS ${RUST_LIB_DIR}/src/lib.rs ${RUST_LIB_DIR}/Cargo.toml
    COMMENT "Building Rust library for CM4"
)

add_custom_target(rust_lib_cm4 ALL
    DEPENDS ${RUST_TARGET_DIR}/${RUST_TARGET}/release/librust_openamp_cm4.a
)

# Link Rust library
target_link_libraries(app PRIVATE ${RUST_TARGET_DIR}/${RUST_TARGET}/release/librust_openamp_cm4.a)
