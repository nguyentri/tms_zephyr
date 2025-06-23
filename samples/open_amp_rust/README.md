# OpenAMP Rust Sample: Building for CM7 and CM4

This sample supports building for either core independently, or both together using sysbuild.

## Directory Structure
- `cm7/` : Application for Cortex-M7
- `cm4/` : Application for Cortex-M4

## Build Commands

| Build Target | Command | Result |
|--------------|---------|--------|
| CM7 only | `west build -b mimxrt1160_evk/mimxrt1166/cm7 samples/open_amp_rust/cm7 -p always` | Only CM7 image is built |
| CM4 only | `west build -b mimxrt1160_evk/mimxrt1166/cm4 samples/open_amp_rust/cm4 -p always` | Only CM4 image is built |
| Both (multi) | `west build -b mimxrt1160_evk/mimxrt1166/cm7 --sysbuild samples/open_amp_rust/cm7 -p always` | Both CM7 and CM4 images built |

## Notes
- Do **not** use `--sysbuild` for single-core builds.
- Each core's application is self-contained and does not depend on sysbuild for single-core builds.
- For multi-core builds, ensure sysbuild files are present and use the `--sysbuild` option.

## Requirements
- Zephyr SDK and west tool
- Board: NXP MIMXRT1160-EVK

## Serial Console
- Speed: 115200
- Data: 8 bits
- Parity: None
- Stop bits: 1

---
For more details, see the documentation in each core's folder.
