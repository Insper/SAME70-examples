#![no_std]
#![no_main]

use panic_halt as _;
use cortex_m_rt::entry;
use cortex_m::asm;

struct Pio {
    pio_per: *mut u32, // PIO Enable Register
    pio_oer: *mut u32, // PIO Output Enable Register
    pio_sodr: *mut u32, // Set Output Data Register
    pio_codr: *mut u32, // Clear Output Data Register
    pio_puer: *mut u32, // Pull-up Enable Register
    pio_pudr: *mut u32, // Pull-up Disable Register
    pio_ier: *mut u32, // Interrupt Enable Register
    pio_ifscer: *mut u32, // Input Filter Slow Clock Enable Register
    pio_ifscdr: *mut u32, // Input Filter Slow Clock Disable Register 
    pio_mder: *mut u32, // Multi-driver Enable Register 
    pio_mddr: *mut u32, // Multi-driver Disable Register
}

fn pio_set(p_pio: &Pio, ul_mask: u32) {
    unsafe { *(p_pio.pio_sodr)= ul_mask };
}

fn pio_clear(p_pio: &Pio, ul_mask: u32) {
    unsafe { *(p_pio.pio_codr)= ul_mask };
}

fn pio_pull_up(p_pio: &Pio, ul_mask: u32, ul_pull_up_enable: bool) {
    if ul_pull_up_enable {
        unsafe { *(p_pio.pio_puer) = ul_mask };
    } else {
        unsafe { *(p_pio.pio_pudr) = ul_mask };
    }
}

fn pio_set_input(p_pio: &Pio, ul_mask: u32, ul_attribute: bool) {
    unsafe { *(p_pio.pio_ier) = ul_mask };

    if ul_attribute {
        pio_pull_up(p_pio, ul_mask, true);
        unsafe { *(p_pio.pio_ifscer) = ul_mask };
    } else {
        pio_pull_up(p_pio, ul_mask, false);
        unsafe { *(p_pio.pio_ifscdr) = ul_mask };
    }
}

fn pio_set_output(
    p_pio: &Pio, 
    ul_mask: u32, 
    ul_default_level: bool, 
    ul_multidrive_enable: bool, 
    ul_pull_up_enable: bool) 
{

    unsafe { *(p_pio.pio_per) = ul_mask };
    unsafe { *(p_pio.pio_oer) = ul_mask };

    if ul_default_level {
        pio_set(p_pio, ul_mask);
    } else {
        pio_clear(p_pio, ul_mask);
    }

    if ul_multidrive_enable {
        unsafe { *(p_pio.pio_mder) = ul_mask };
    } else {
        unsafe { *(p_pio.pio_mddr) = ul_mask };
    }

    pio_pull_up(p_pio, ul_mask, ul_pull_up_enable);
}

fn delay_ms(ms: u32) {
    for _wait in 0..ms {
        asm::nop();
    }
}

#[entry]
fn main() -> ! {

    let pioc = Pio {
        pio_per: 0x400e1200 as *mut u32,
        pio_oer: 0x400e1210 as *mut u32,
        pio_sodr: 0x400E1230 as *mut u32,
        pio_codr: 0x400E1234 as *mut u32,
        pio_puer: 0x400E1264 as *mut u32,
        pio_pudr: 0x400E1260 as *mut u32,
        pio_ier: 0x400E1240 as *mut u32,
        pio_ifscer: 0x400E1284 as *mut u32,
        pio_ifscdr: 0x400E1280 as *mut u32,
        pio_mder: 0x400E1250 as *mut u32,
        pio_mddr: 0x400E1254 as *mut u32,
    };

    let ul_pioc8_mask: u32 = 1 << 8;

    let pioa = Pio {
        pio_per: 0x400E0E00 as *mut u32,
        pio_oer: 0x400E0E10 as *mut u32,
        pio_sodr: 0x400E0E30 as *mut u32,
        pio_codr: 0x400E0E34 as *mut u32,
        pio_puer: 0x400E0E64 as *mut u32,
        pio_pudr: 0x400E0E60 as *mut u32,
        pio_ier: 0x400E0E40 as *mut u32,
        pio_ifscer: 0x400E0E84 as *mut u32,
        pio_ifscdr: 0x400E0E80 as *mut u32,
        pio_mder: 0x400E0E50 as *mut u32,
        pio_mddr: 0x400E0E54 as *mut u32,
    };

    let ul_pioa11_mask: u32 = 1 << 11;

    let wdt_mr: *mut u32  = 0x400e1854 as *mut u32;

    pio_set_output(&pioc, ul_pioc8_mask, false, false, false);
    pio_set_input(&pioa, ul_pioa11_mask, false);


    unsafe {
        *wdt_mr = 0x000080000; // Disable watchdog timer.
    }

    loop {
        pio_clear(&pioc, ul_pioc8_mask);

        delay_ms(100000);

        pio_set(&pioc, ul_pioc8_mask);

        delay_ms(100000);
    }
}
