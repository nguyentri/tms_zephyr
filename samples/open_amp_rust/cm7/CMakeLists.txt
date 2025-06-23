
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(open_amp_rust_cm7)

# Add Rust support
find_program(CARGO cargo REQUIRED)

# Check if we're in a sysbuild context
if(DEFINED SYSBUILD_PROJECT OR DEFINED SB_CONFIG_REMOTE_BOARD)
    set(CONFIG_OPENAMP_SYSBUILD ON)
    # Include sysbuild-generated files
    target_include_directories(app PRIVATE ${CMAKE_BINARY_DIR}/zephyr/include/generated)
else()
    set(CONFIG_OPENAMP_SYSBUILD OFF)
endif()

# Add source files
target_sources(app PRIVATE src/main.c)

# Rust library configuration
set(RUST_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rust_lib)
set(RUST_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/rust_target)

# Build Rust library
add_custom_command(
    OUTPUT ${RUST_TARGET_DIR}/thumbv7em-none-eabihf/release/librust_openamp.a
    COMMAND ${CMAKE_COMMAND} -E env
        CARGO_TARGET_DIR=${RUST_TARGET_DIR}
        ${CARGO} build --release --target thumbv7em-none-eabihf
    WORKING_DIRECTORY ${RUST_LIB_DIR}
    DEPENDS ${RUST_LIB_DIR}/src/lib.rs ${RUST_LIB_DIR}/Cargo.toml
    COMMENT "Building Rust library"
)

add_custom_target(rust_lib ALL
    DEPENDS ${RUST_TARGET_DIR}/thumbv7em-none-eabihf/release/librust_openamp.a
)

# Link Rust library
target_link_libraries(app PRIVATE ${RUST_TARGET_DIR}/thumbv7em-none-eabihf/release/librust_openamp.a)
add_dependencies(app rust_lib)
