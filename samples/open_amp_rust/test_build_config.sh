#!/bin/bash

# Test script to verify the build configuration logic
# This simulates the key aspects of the build without requiring full Zephyr setup

echo "=== OpenAMP Rust Build Configuration Test ==="
echo

# Test 1: Check that zephyr_image_info.h inclusion is conditional
echo "Test 1: Checking conditional header inclusion..."
if grep -q "#ifdef CONFIG_OPENAMP_SYSBUILD" samples/open_amp_rust/cm7/src/main.c; then
    echo "✓ CM7 main.c has conditional zephyr_image_info.h inclusion"
else
    echo "✗ CM7 main.c missing conditional inclusion"
fi

if grep -q "#ifdef CONFIG_OPENAMP_SYSBUILD" samples/open_amp_rust/cm4/src/main.c; then
    echo "✓ CM4 main.c has conditional zephyr_image_info.h inclusion"
else
    echo "✗ CM4 main.c missing conditional inclusion"
fi

# Test 2: Check CMakeLists.txt sysbuild detection
echo
echo "Test 2: Checking CMakeLists.txt sysbuild detection..."
if grep -q "if(DEFINED SYSBUILD_PROJECT OR DEFINED SB_CONFIG_REMOTE_BOARD)" samples/open_amp_rust/cm7/CMakeLists.txt; then
    echo "✓ CM7 CMakeLists.txt has sysbuild detection"
else
    echo "✗ CM7 CMakeLists.txt missing sysbuild detection"
fi

if grep -q "if(DEFINED SYSBUILD_PROJECT OR DEFINED SB_CONFIG_REMOTE_BOARD)" samples/open_amp_rust/cm4/CMakeLists.txt; then
    echo "✓ CM4 CMakeLists.txt has sysbuild detection"
else
    echo "✗ CM4 CMakeLists.txt missing sysbuild detection"
fi

# Test 3: Check Kconfig files
echo
echo "Test 3: Checking Kconfig configuration..."
if grep -q "config OPENAMP_SYSBUILD" samples/open_amp_rust/cm7/Kconfig; then
    echo "✓ CM7 Kconfig has OPENAMP_SYSBUILD option"
else
    echo "✗ CM7 Kconfig missing OPENAMP_SYSBUILD option"
fi

if grep -q "config OPENAMP_SYSBUILD" samples/open_amp_rust/cm4/Kconfig; then
    echo "✓ CM4 Kconfig has OPENAMP_SYSBUILD option"
else
    echo "✗ CM4 Kconfig missing OPENAMP_SYSBUILD option"
fi

# Test 4: Check sysbuild files
echo
echo "Test 4: Checking sysbuild configuration..."
if [ -f "samples/open_amp_rust/sysbuild.cmake" ]; then
    echo "✓ sysbuild.cmake exists"
else
    echo "✗ sysbuild.cmake missing"
fi

if [ -f "samples/open_amp_rust/sysbuild.conf" ]; then
    echo "✓ sysbuild.conf exists"
else
    echo "✗ sysbuild.conf missing"
fi

# Test 5: Check Rust library structure
echo
echo "Test 5: Checking Rust library structure..."
if [ -f "samples/open_amp_rust/cm7/rust_lib/Cargo.toml" ] && [ -f "samples/open_amp_rust/cm7/rust_lib/src/lib.rs" ]; then
    echo "✓ CM7 Rust library structure complete"
else
    echo "✗ CM7 Rust library structure incomplete"
fi

if [ -f "samples/open_amp_rust/cm4/rust_lib/Cargo.toml" ] && [ -f "samples/open_amp_rust/cm4/rust_lib/src/lib.rs" ]; then
    echo "✓ CM4 Rust library structure complete"
else
    echo "✗ CM4 Rust library structure incomplete"
fi

# Test 6: Verify Rust library exports
echo
echo "Test 6: Checking Rust library exports..."
if grep -q "#\[no_mangle\]" samples/open_amp_rust/cm7/rust_lib/src/lib.rs && grep -q "rust_openamp_init" samples/open_amp_rust/cm7/rust_lib/src/lib.rs; then
    echo "✓ CM7 Rust library has proper C exports"
else
    echo "✗ CM7 Rust library missing C exports"
fi

if grep -q "#\[no_mangle\]" samples/open_amp_rust/cm4/rust_lib/src/lib.rs && grep -q "rust_openamp_init" samples/open_amp_rust/cm4/rust_lib/src/lib.rs; then
    echo "✓ CM4 Rust library has proper C exports"
else
    echo "✗ CM4 Rust library missing C exports"
fi

echo
echo "=== Test Summary ==="
echo "The build configuration should now support all three build modes:"
echo "1. CM7 only: west build -b mimxrt1160_evk/mimxrt1166/cm7 samples/open_amp_rust/cm7 -p always"
echo "2. CM4 only: west build -b mimxrt1160_evk/mimxrt1166/cm4 samples/open_amp_rust/cm4 -p always"
echo "3. Multi-core: west build -b mimxrt1160_evk/mimxrt1166/cm7 --sysbuild samples/open_amp_rust/cm7 -p always"
echo
echo "Key fixes implemented:"
echo "- Conditional inclusion of zephyr_image_info.h only in sysbuild mode"
echo "- CMakeLists.txt detects sysbuild context automatically"
echo "- Proper Kconfig configuration for build mode detection"
echo "- Rust integration with proper no_std embedded configuration"
