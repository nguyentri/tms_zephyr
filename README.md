# MIMXRT1160-EVK Dual-Core Transformer Gateway Project

## Prerequisites

1. Install Zephyr SDK
2. Install West tool
3. Install required dependencies

## Setup Instructions

### 1. Clone the Repository
```bash
git clone <repository-url>
cd transformer-gateway
```

### 2. Initialize West Workspace
```bash
# Initialize west manifest
west init -m <repository-url> .

# Fetch all dependencies
west update
```

### 3. Set Up Zephyr Environment
```bash
# Source Zephyr environment
source 3rd_parties/zephyr/zephyr-env.sh
```

### 4. Build Sample Application
```bash
# Build the sample application for MIMXRT1160-EVK
west build -b mimxrt1160_evk/mimxrt1166/cm7 --sysbuild samples/<sample_name> -p always
For example, to build the hello world sample:
west build -b mimxrt1160_evk/mimxrt1166/cm7 samples/hello_world -p always
```

### 4. Build Project

#### Multi-Core Build (Recommended)
```bash
# Build both M4 and M7 cores
west build -b mimxrt1160_evk/mimxrt1166/cm7 --sysbuild
```

#### Single-Core Builds (Development)
```bash
# Build M4 Core
west build -b mimxrt1160_evk/mimxrt1166/cm4 blue_leap/app_m4

# Build M7 Core
west build -b mimxrt1160_evk/mimxrt1166/cm7 blue_leap/app_m7
```

### 5. Flash the Device
```bash
# Flash both cores (M4 first, then M7)
west flash
```

## Troubleshooting

- Ensure all dependencies are correctly fetched with `west update`
- Check Zephyr SDK version compatibility
- Verify board support package for MIMXRT1160-EVK

## Project Structure

```
.
├── 3rd_parties/           # Third-party dependencies
│   ├── zephyr/
│   ├── hal_nxp/
│   ├── open-amp/
│   └── ...
├── blue_leap/
│   ├── app_isw/           # Integrated Software Library
│   ├── app_m4/            # M4 Core Application
│   └── app_m7/            # M7 Core Application
├── CMakeLists.txt         # Top-level build configuration
├── west-manifest/
│   └── west.yml           # West manifest file
└── sysbuild.conf          # Multi-image build configuration
```

## Supported Configurations

- Board: MIMXRT1160-EVK
- Cores: Cortex-M7 + Cortex-M4
- RTOS: Zephyr v4.1.0

## License

[Specify your project license]