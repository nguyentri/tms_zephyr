
# OpenAMP Rust Sample

This sample demonstrates OpenAMP communication between CM7 and CM4 cores on the MIMXRT1160 EVK, with Rust integration.

## Build Modes

This project supports three different build modes:

### 1. CM7 Only (Single Core)
```bash
west build -b mimxrt1160_evk/mimxrt1166/cm7 samples/open_amp_rust/cm7 -p always
```

### 2. CM4 Only (Single Core)
```bash
west build -b mimxrt1160_evk/mimxrt1166/cm4 samples/open_amp_rust/cm4 -p always
```

### 3. Multi-Core (Sysbuild)
```bash
west build -b mimxrt1160_evk/mimxrt1166/cm7 --sysbuild samples/open_amp_rust/cm7 -p always
```

## Key Features

- **Conditional Compilation**: The project automatically detects whether it's being built in sysbuild mode or single-core mode
- **Rust Integration**: Both CM7 and CM4 applications include Rust libraries for OpenAMP functionality
- **Proper Header Handling**: `zephyr_image_info.h` is only included when building with sysbuild

## Technical Details

### Build Configuration

The main fix for the "zephyr_image_info.h: No such file or directory" error is implemented through:

1. **CMakeLists.txt Detection**: Both CM7 and CM4 CMakeLists.txt files detect sysbuild mode:
   ```cmake
   if(DEFINED SYSBUILD_PROJECT OR DEFINED SB_CONFIG_REMOTE_BOARD)
       set(CONFIG_OPENAMP_SYSBUILD ON)
       target_include_directories(app PRIVATE ${CMAKE_BINARY_DIR}/zephyr/include/generated)
   else()
       set(CONFIG_OPENAMP_SYSBUILD OFF)
   endif()
   ```

2. **Conditional Header Inclusion**: Source files only include sysbuild-generated headers when appropriate:
   ```c
   #ifdef CONFIG_OPENAMP_SYSBUILD
   #include <zephyr_image_info.h>
   #endif
   ```

3. **Kconfig Configuration**: Custom Kconfig option to control sysbuild-specific features.

### Rust Integration

- Each core (CM7/CM4) has its own Rust library in the `rust_lib` directory
- Rust libraries are compiled as static libraries and linked with the C application
- Rust code provides OpenAMP functionality with proper `no_std` embedded configuration

## Prerequisites

- Zephyr SDK
- Rust toolchain with `thumbv7em-none-eabihf` target
- West build tool

## Installation

1. Install Rust target:
   ```bash
   rustup target add thumbv7em-none-eabihf
   ```

2. Build using one of the three modes listed above.

## Troubleshooting

- If you get "zephyr_image_info.h not found", ensure you're using the correct build command for your intended mode
- For multi-core builds, always use the `--sysbuild` flag
- For single-core builds, do NOT use the `--sysbuild` flag
