
# MIMXRT1160-EVK Dual-Core Transformer Gateway Project

---

## Overview

This project demonstrates a dual-core application for the NXP MIMXRT1160-EVK board using Zephyr RTOS. It leverages the Cortex-M7 and Cortex-M4 cores to run coordinated tasks, showcasing multi-core communication and processing.

---

## Prerequisites

Before you begin, ensure you have the following installed and configured:

- **Zephyr SDK** (compatible version)
- **West tool** (Zephyr's meta-tool for managing projects)
- Required system dependencies (e.g., Python, CMake, Ninja, device drivers)

Refer to the [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/getting_started/index.html) for detailed installation instructions.

---

## Setup Instructions

### 1. Clone the Repository

```bash
git clone <repository-url>
cd tms-zephyr
```

### 2. Initialize and Update West Workspace

```bash
# Initialize west with the project manifest
west init -m <repository-url> .

# Fetch all project dependencies
west update
```

### 3. Set Up Zephyr Environment

```bash
# Source the Zephyr environment variables
source 3rd_parties/zephyr/zephyr-env.sh
```

### 4. Build Sample Application

To build a sample application, specify the board, core, and sample path:

```bash
west build -b mimxrt1160_evk/mimxrt1166/cm7 --sysbuild samples/<sample_name>/<core_name> -p always
```

For example, to build the OpenAMP Rust "hello world" sample on the Cortex-M7 core:

```bash
west build -b mimxrt1160_evk/mimxrt1166/cm7 --sysbuild samples/open_amp_rust/cm7 -p always
```

### 5. Build the Full Project

#### Multi-Core Build (Recommended)

```bash
# west build -b mimxrt1160_evk/mimxrt1166/cm7 blue_leap/
west build -b mimxrt1160_evk/mimxrt1166/cm7 blue_leap/cm7
```

#### Single-Core Builds (Not Recommended / Not Supported)

```bash
# Build Cortex-M4 core application
west build -b mimxrt1160_evk/mimxrt1166/cm4 blue_leap/cm4

# Build Cortex-M7 core application
west build -b mimxrt1160_evk/mimxrt1166/cm7 blue_leap/cm7
```

### 6. Flash the Device

```bash
west flash
```

---

## Troubleshooting

- Run `west update` to ensure all dependencies are up to date.
- Verify your Zephyr SDK version matches the project requirements.
- Confirm the MIMXRT1160-EVK board support package is correctly installed.
- Check environment variables are properly sourced.
- For build errors, clean the build directory with `west build -t clean` and rebuild.

---

## Project Structure

```
.
├── 3rd_parties/           # Third-party dependencies (Zephyr, HAL, OpenAMP, etc.)
│   ├── zephyr/
│   ├── hal_nxp/
│   ├── open-amp/
│   └── ...
├── samples/               # Sample applications shared between cores
│   ├── open_amp_rust/     # OpenAMP Rust sample
│   │   ├── cm4/           # Cortex-M4 specific code
│   │   ├── cm7/           # Cortex-M7 specific code
│   │   └── common/        # Shared code for both cores
├── blue_leap/             # Project-specific applications
│   ├── cm4/               # Cortex-M4 core code
│   ├── cm7/               # Cortex-M7 core code
│   └── common/            # Shared code for both cores
├── CMakeLists.txt         # Top-level build configuration
├── west-manifest/
│   └── west.yml           # West manifest file for dependencies
└── sysbuild.conf          # Multi-image build configuration
```

---

## Supported Configurations

- **Board:** MIMXRT1160-EVK
- **Cores:** Dual-core (Cortex-M7 + Cortex-M4)
- **RTOS:** Zephyr v4.1.0

---

## License

[Specify your project license here, e.g., MIT, Apache 2.0, etc.]

---
