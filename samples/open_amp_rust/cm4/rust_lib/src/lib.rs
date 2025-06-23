
#![no_std]

use core::ffi::{c_char, c_int};

// External C functions that we'll call from Zephyr
extern "C" {
    fn printk(fmt: *const c_char, ...);
}

// Global state for OpenAMP CM4
static mut INITIALIZED: bool = false;
static mut MESSAGE_COUNT: u32 = 0;

#[no_mangle]
pub extern "C" fn rust_openamp_init_cm4() {
    unsafe {
        if !INITIALIZED {
            let msg = b"Rust OpenAMP library initialized on CM4\n\0";
            printk(msg.as_ptr() as *const c_char);
            INITIALIZED = true;
            MESSAGE_COUNT = 0;
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_openamp_send_message_cm4(message: *const c_char) -> c_int {
    unsafe {
        if message.is_null() {
            return -1;
        }

        // Calculate message length
        let mut len = 0;
        let mut ptr = message;
        while *ptr != 0 {
            len += 1;
            ptr = ptr.add(1);
        }

        MESSAGE_COUNT += 1;

        let log_msg = b"Rust CM4: Preparing to send message #%d (len=%d)\n\0";
        printk(log_msg.as_ptr() as *const c_char, MESSAGE_COUNT, len);

        // In a real implementation, you would call the actual IPC service
        // For now, we'll just log that we're sending
        0
    }
}

#[no_mangle]
pub extern "C" fn rust_openamp_process_received_cm4(data: *const c_char, len: usize) {
    unsafe {
        if data.is_null() || len == 0 {
            return;
        }

        let log_msg = b"Rust CM4: Processing received message (len=%d)\n\0";
        printk(log_msg.as_ptr() as *const c_char, len);

        // Process the received message
        // In a real implementation, you might parse the message,
        // update state, or trigger responses
    }
}

#[no_mangle]
pub extern "C" fn rust_openamp_get_stats_cm4() -> u32 {
    unsafe {
        MESSAGE_COUNT
    }
}

// Panic handler for no_std environment
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    unsafe {
        let msg = b"Rust CM4 panic occurred!\n\0";
        printk(msg.as_ptr() as *const c_char);
    }
    loop {}
}
