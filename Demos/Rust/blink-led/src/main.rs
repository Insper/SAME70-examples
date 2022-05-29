#![no_std]
#![no_main]

use panic_halt as _;
use cortex_m_rt::entry;
use cortex_m::asm;

#[entry]
fn main() -> ! {

    let pioc_per: *mut i32  = 0x400e1200 as *mut i32;
    let pioc_oer: *mut i32  = 0x400e1210 as *mut i32;
    let pioc_odsr: *mut i32  = 0x400e1238 as *mut i32;
    let pioc_ower: *mut i32  = 0x400e12a0 as *mut i32;
    let pioc_owdr: *mut i32  = 0x400e12a4 as *mut i32;
    let wdt_mr: *mut i32  = 0x400e1854 as *mut i32;

    unsafe {
        *wdt_mr = 0x000080000; // Disable watchdog timer.

        *pioc_per = 0x100;
        *pioc_oer = 0x100;
        *pioc_owdr = 0;
        *pioc_ower = 0x100;
    }

    loop {
        unsafe {
            *pioc_odsr  ^= 0x100;
        }
        
        asm::delay(10000000);
    }
}
