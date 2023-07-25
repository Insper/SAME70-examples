/**
 *
 * \file
 *
 * \brief This module contains SAME70 BSP APIs implementation.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include "bsp/include/nm_bsp.h"
#include "common/include/nm_common.h"
#include "asf.h"
#include "conf_winc.h"
//CORSI freertos
#include "semphr.h"

static tpfNmBspIsr gpfIsr;

//CORSI freertos
extern SemaphoreHandle_t xSemaphore;
extern TaskHandle_t xHandleWifi;

static void chip_isr(uint32_t id, uint32_t mask)
{
  //CORSI acorda task wifi freertos
  //BaseType_t xHigherPriorityTaskWoken = pdTRUE;
  //xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
  //vTaskNotifyGiveFromISR( xHandleWifi, &xHigherPriorityTaskWoken );
  
	if ((id == CONF_WINC_SPI_INT_PIO_ID) && (mask == CONF_WINC_SPI_INT_MASK)) {
		if (gpfIsr) {
			gpfIsr();
		}
	}
}

/*
 *	@fn		init_chip_pins
 *	@brief	Initialize reset, chip enable and wake pin
 */
static void init_chip_pins(void)
{
	ioport_init();
	ioport_set_pin_dir(CONF_WINC_PIN_RESET, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(CONF_WINC_PIN_RESET, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(CONF_WINC_PIN_CHIP_ENABLE, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(CONF_WINC_PIN_CHIP_ENABLE, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(CONF_WINC_PIN_WAKE, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(CONF_WINC_PIN_WAKE, IOPORT_PIN_LEVEL_HIGH);
}

/*
*	@fn		nm_bsp_init
*	@brief	Initialize BSP
*	@return	0 in case of success and -1 in case of failure
*/
sint8 nm_bsp_init(void)
{
	gpfIsr = NULL;

	/* Initialize chip IOs. */
	init_chip_pins();

    /* Make sure a 1ms Systick is configured. */
    if (!(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk && SysTick->CTRL & SysTick_CTRL_TICKINT_Msk)) {
	    delay_init();
    }

	/* Perform chip reset. */
	nm_bsp_reset();

	return 0;
}

/**
 *	@fn		nm_bsp_reset
 *	@brief	Reset WINC1500 SoC by setting CHIP_EN and RESET_N signals low,
 *           CHIP_EN high then RESET_N high
 */
void nm_bsp_reset(void)
{
	ioport_set_pin_level(CONF_WINC_PIN_CHIP_ENABLE, IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(CONF_WINC_PIN_RESET, IOPORT_PIN_LEVEL_LOW);
	nm_bsp_sleep(100);
	ioport_set_pin_level(CONF_WINC_PIN_CHIP_ENABLE, IOPORT_PIN_LEVEL_HIGH);
	nm_bsp_sleep(100);
	ioport_set_pin_level(CONF_WINC_PIN_RESET, IOPORT_PIN_LEVEL_HIGH);
	nm_bsp_sleep(100);
}

/*
*	@fn		nm_bsp_sleep
*	@brief	Sleep in units of mSec
*	@param[IN]	u32TimeMsec
*				Time in milliseconds
*/
void nm_bsp_sleep(uint32 u32TimeMsec)
{
	while(u32TimeMsec--) {
		delay_ms(4);
	}
}

/*
*	@fn		nm_bsp_register_isr
*	@brief	Register interrupt service routine
*	@param[IN]	pfIsr
*				Pointer to ISR handler
*/
void nm_bsp_register_isr(tpfNmBspIsr pfIsr)
{
	gpfIsr = pfIsr;

	/* Configure PGIO pin for interrupt from SPI slave, used when slave has data to send. */
	pmc_enable_periph_clk(CONF_WINC_SPI_INT_PIO_ID);
	pio_configure_pin(CONF_WINC_SPI_INT_PIN, PIO_TYPE_PIO_INPUT);
	pio_pull_up(CONF_WINC_SPI_INT_PIO, CONF_WINC_SPI_INT_MASK, PIO_PULLUP);
	/*Interrupt on falling edge*/
	pio_handler_set(CONF_WINC_SPI_INT_PIO, CONF_WINC_SPI_INT_PIO_ID,
	CONF_WINC_SPI_INT_MASK, PIO_PULLUP | PIO_IT_FALL_EDGE, chip_isr);
	pio_enable_interrupt(CONF_WINC_SPI_INT_PIO, CONF_WINC_SPI_INT_MASK);
  // corsi: clear PIO IRQ before enable nvic
  pio_get_interrupt_status(CONF_WINC_SPI_INT_PIO);
  // corsi: for use with RTOS priority shall be < configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 
 	NVIC_SetPriority((IRQn_Type) CONF_WINC_SPI_INT_PIO_ID, 5);
 	NVIC_EnableIRQ((IRQn_Type) CONF_WINC_SPI_INT_PIO_ID);
	pio_handler_set_priority(CONF_WINC_SPI_INT_PIO, (IRQn_Type)CONF_WINC_SPI_INT_PIO_ID,
			CONF_WINC_SPI_INT_PRIORITY);
}

/*
*	@fn		nm_bsp_interrupt_ctrl
*	@brief	Enable/Disable interrupts
*	@param[IN]	u8Enable
*				'0' disable interrupts. '1' enable interrupts
*/
void nm_bsp_interrupt_ctrl(uint8 u8Enable)
{
	if (u8Enable) {
		pio_enable_interrupt(CONF_WINC_SPI_INT_PIO, CONF_WINC_SPI_INT_MASK);
	}
	else {
		pio_disable_interrupt(CONF_WINC_SPI_INT_PIO, CONF_WINC_SPI_INT_MASK);
	}
}

