//! CM7-specific Rust OpenAMP library implementation.
//!
//! This library provides a Rust interface for OpenAMP communication on the CM7 core
//! of the MIMXRT1160-EVK platform. It handles message sending, receiving, and statistics
//! tracking for inter-processor communication between the CM7 and CM4 cores.

#![no_std]

use core::ffi::{c_char, c_int};
use core::panic::PanicInfo;
use core::sync::atomic::{AtomicBool, AtomicU32, Ordering};

extern "C" {
    fn printk(fmt: *const c_char, ...);
}

static INITIALIZED: AtomicBool = AtomicBool::new(false);
static MESSAGE_COUNT: AtomicU32 = AtomicU32::new(0);

/// Initializes the Rust OpenAMP library for the CM7 core.
///
/// This function should be called once during system initialization before
/// any other OpenAMP operations are performed.
#[no_mangle]
pub extern "C" fn rust_openamp_init() {
    if INITIALIZED.load(Ordering::Relaxed) {
        return;
    }
    unsafe {
        printk("Rust OpenAMP library initialized for CM7\n\r".as_ptr() as *const c_char);
    }
    INITIALIZED.store(true, Ordering::Relaxed);
}

/// Sends a message from the CM7 core.
///
/// This function logs the message being sent and increments the message counter.
/// The actual IPC transmission is handled by the C code.
///
/// # Arguments
///
/// * `message` - A pointer to a null-terminated C-style string containing the message to send.
///
/// # Returns
///
/// * `0` on success
/// * `-1` if the library is not initialized
///
/// # Safety
///
/// The caller must ensure that `message` points to a valid null-terminated string.
#[no_mangle]
pub extern "C" fn rust_openamp_send_message(message: *const c_char) -> c_int {
    if !INITIALIZED.load(Ordering::Relaxed) {
        return -1;
    }

    // Calculate message length by finding the null terminator
    let mut len = 0;
    while unsafe { *message.add(len) } != 0 {
        len += 1;
    }

    unsafe {
        printk(
            "CM7 sending message: %s (len: %d)\n\r".as_ptr() as *const c_char,
            message,
            len,
        );
    }

    // Increment message count atomically
    MESSAGE_COUNT.fetch_add(1, Ordering::Relaxed);

    0
}

/// Processes a message received by the CM7 core.
///
/// This function is called when a message is received from the CM4 core.
/// It logs the received message length for debugging purposes.
///
/// # Arguments
///
/// * `data` - A pointer to the received data buffer.
/// * `len` - The length of the received data in bytes.
///
/// # Safety
///
/// The caller must ensure that `data` points to a valid buffer of at least `len` bytes.
#[no_mangle]
#[allow(unused_variables)]
pub extern "C" fn rust_openamp_process_received(data: *const c_char, len: usize) {
    if !INITIALIZED.load(Ordering::Relaxed) {
        return;
    }

    unsafe {
        printk(
            "CM7 received message with length: %d\n\r".as_ptr() as *const c_char,
            len,
        );
        // Note: We are not printing the message content here to avoid potential
        // issues with non-null-terminated strings. A safer way would be to
        // copy the data to a buffer and null-terminate it before printing.
    }
}

/// Gets the number of messages sent from the CM7 core.
///
/// # Returns
///
/// The total number of messages sent since initialization.
#[no_mangle]
pub extern "C" fn rust_openamp_get_stats() -> u32 {
    MESSAGE_COUNT.load(Ordering::Relaxed)
}

/// Panic handler for the no_std environment.
///
/// This function is called when a panic occurs in the Rust code.
/// It prints an error message and enters an infinite loop.
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    unsafe {
        printk("Rust panic on CM7!\n\r".as_ptr() as *const c_char);
    }
    loop {}
}