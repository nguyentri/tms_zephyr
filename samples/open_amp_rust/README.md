# OpenAMP Rust Sample for MIMXRT1160-EVK

This sample demonstrates inter-processor communication (IPC) between the CM7 and CM4 cores of the MIMXRT1160-EVK using Zephyr's IPC service with Rust integration.

## Overview

The sample consists of two applications:
- **CM7 Application**: Acts as the primary core, initializes the IPC service and communicates with the CM4 core
- **CM4 Application**: Acts as the secondary core, registers an endpoint and exchanges messages with the CM7 core

Both applications integrate Rust libraries that handle message logging, counting, and processing.

## Architecture

```
┌─────────────────┐    IPC Service     ┌─────────────────┐
│   CM7 Core      │◄──────────────────►│   CM4 Core      │
│                 │                    │                 │
│ ┌─────────────┐ │                    │ ┌─────────────┐ │
│ │ C Main App  │ │                    │ │ C Main App  │ │
│ │             │ │                    │ │             │ │
│ │ ┌─────────┐ │ │                    │ │ ┌─────────┐ │ │
│ │ │ Rust    │ │ │                    │ │ │ Rust    │ │ │
│ │ │ Library │ │ │                    │ │ │ Library │ │ │
│ │ └─────────┘ │ │                    │ │ └─────────┘ │ │
│ └─────────────┘ │                    │ └─────────────┘ │
└─────────────────┘                    └─────────────────┘
```

## Key Features

- **Shared C Header**: Common definitions and function prototypes in `common/include/openamp_common.h`
- **Rust Integration**: Both cores use Rust libraries for message handling and statistics
- **Proper IPC Service Usage**: Uses Zephyr's modern IPC service API with semaphore-based synchronization
- **Error Handling**: Comprehensive error checking and logging
- **Atomic Operations**: Thread-safe message counting using atomic operations in Rust

## File Structure

```
samples/open_amp_rust/
├── common/
│   └── include/
│       └── openamp_common.h          # Shared C definitions
├── cm4/
│   ├── src/
│   │   └── main.c                    # CM4 main application
│   ├── rust_lib/
│   │   ├── src/
│   │   │   └── lib.rs                # CM4 Rust library
│   │   └── Cargo.toml                # Rust package configuration
│   ├── CMakeLists.txt                # CM4 build configuration
│   └── prj.conf                      # CM4 Zephyr configuration
└── cm7/
    ├── src/
    │   └── main.c                    # CM7 main application
    ├── rust_lib/
    │   ├── src/
    │   │   └── lib.rs                # CM7 Rust library
    │   └── Cargo.toml                # Rust package configuration
    ├── CMakeLists.txt                # CM7 build configuration
    └── prj.conf                      # CM7 Zephyr configuration
```

## Building

### Prerequisites

1. Zephyr SDK installed
2. West tool installed
3. Rust toolchain with `thumbv7m-none-eabi` target
4. Cargo installed

### Build Commands

```bash
# Build both cores using sysbuild (recommended)
west build -b mimxrt1160_evk/mimxrt1166/cm7 --sysbuild samples/open_amp_rust -p always

# Or build individual cores
west build -b mimxrt1160_evk/mimxrt1166/cm7 samples/open_amp_rust/cm7 -p always
west build -b mimxrt1160_evk/mimxrt1166/cm4 samples/open_amp_rust/cm4 -p always
```

## Configuration

### Zephyr Configuration (prj.conf)

Both cores are configured with:
- OpenAMP support (`CONFIG_OPENAMP=y`)
- IPC service with RPMSG backend (`CONFIG_IPC_SERVICE=y`, `CONFIG_IPC_SERVICE_BACKEND_RPMSG=y`)
- Mailbox support for inter-core communication (`CONFIG_MBOX=y`)
- Logging support for debugging (`CONFIG_LOG=y`)

### Rust Configuration (Cargo.toml)

- Target: `thumbv7m-none-eabi` (Cortex-M4/M7 without hardware floating point)
- Library type: `staticlib` for linking with C code
- Optimized for size (`opt-level = "s"`)
- Panic handling configured for embedded environment

## Code Improvements

This sample includes several improvements over basic OpenAMP examples:

### 1. Shared Header File
- Eliminates code duplication between cores
- Centralizes function prototypes and includes
- Makes maintenance easier

### 2. Proper IPC Service API Usage
- Uses `ipc_service_open_instance()` instead of deprecated functions
- Implements semaphore-based synchronization for endpoint binding
- Proper error handling for all IPC operations

### 3. Enhanced Rust Integration
- Comprehensive documentation with safety notes
- Atomic operations for thread-safe counters
- Proper `no_std` environment setup
- Detailed function documentation

### 4. Robust Error Handling
- All IPC operations check return values
- Comprehensive logging for debugging
- Graceful handling of initialization failures

## Usage

1. Flash the application to the MIMXRT1160-EVK
2. Connect to the serial console (115200 baud) for uart1 (CM7) and uart2 (CM4)
3. Observe the logs for message exchanges between the CM7 and CM4 cores:

```
[00:00:00.123,000] <inf> cm7_main: Starting CM7 application
[00:00:00.124,000] <inf> cm4_main: Starting CM4 application
Rust OpenAMP library initialized for CM7
Rust OpenAMP library initialized for CM4
[00:00:01.125,000] <inf> cm7_main: IPC endpoint bound
[00:00:01.126,000] <inf> cm4_main: IPC endpoint bound
CM7 sending message: Hello from CM7, count: 0 (len: 26)
CM4 received message with length: 26
CM4 sending message: Hello from CM4, count: 0 (len: 26)
CM7 received message with length: 26
...
```

## Troubleshooting

### Build Issues
- Ensure Rust target `thumbv7m-none-eabi` is installed: `rustup target add thumbv7m-none-eabi`
- Verify Zephyr SDK and environment are properly set up
- Check that all dependencies are installed

### Runtime Issues
- Verify device tree configuration includes IPC and mailbox nodes
- Check that both cores are properly flashed
- Ensure serial console is connected to the correct UART

## Extending the Sample

This sample provides a solid foundation for more complex inter-processor communication:

1. **Add Message Types**: Extend the Rust libraries to handle different message types
2. **Implement Protocols**: Add request/response patterns or streaming data
3. **Add Error Recovery**: Implement timeout handling and connection recovery
4. **Performance Monitoring**: Extend statistics collection for throughput analysis
5. **Custom Backends**: Experiment with different IPC service backends

## References

- [Zephyr IPC Service Documentation](https://docs.zephyrproject.org/latest/services/ipc/index.html)
- [MIMXRT1160-EVK Documentation](https://docs.zephyrproject.org/latest/boards/nxp/mimxrt1160_evk/doc/index.html)
- [OpenAMP Documentation](https://openamp.readthedocs.io/)
- [Rust Embedded Book](https://docs.rust-embedded.org/book/)