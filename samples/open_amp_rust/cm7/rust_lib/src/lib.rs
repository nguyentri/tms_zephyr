
#![no_std]

use core::ffi::{c_char, c_int};
use core::ptr;

// External C functions that we'll call from Zephyr
extern "C" {
    fn printk(fmt: *const c_char, ...);
    fn ipc_service_send(instance: *const u8, token: *const u8, data: *const u8, len: usize) -> c_int;
}

// Global state for OpenAMP
static mut INITIALIZED: bool = false;

#[no_mangle]
pub extern "C" fn rust_openamp_init() {
    unsafe {
        if !INITIALIZED {
            let msg = b"Rust OpenAMP library initialized on CM7\n\0";
            printk(msg.as_ptr() as *const c_char);
            INITIALIZED = true;
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_openamp_send_message(message: *const c_char) -> c_int {
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

        let log_msg = b"Rust: Sending message via OpenAMP\n\0";
        printk(log_msg.as_ptr() as *const c_char);

        // In a real implementation, you would call the actual IPC service
        // For now, we'll just log that we're sending
        0
    }
}

// Panic handler for no_std environment
#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    unsafe {
        let msg = b"Rust panic occurred!\n\0";
        printk(msg.as_ptr() as *const c_char);
    }
    loop {}
}
