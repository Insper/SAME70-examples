
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "asf.h"
#include "fifo.h"
#include "heartbeat_4md69.h"

	
/** Reference voltage for AFEC,in mv. */
#define VOLT_REF        (3300)

/** The maximal digital value */
#define MAX_DIGITAL     (4095UL)

#define STRING_EOL    "\r"
#define STRING_HEADER "-- AFEC Temperature Sensor Example --\r\n" \
		"-- "BOARD_NAME" --\r\n" \
		"-- Compiled: "__DATE__" "__TIME__" --"STRING_EOL

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}


/**
 * \brief Application entry point.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system. */
	sysclk_init();
	board_init();
	configure_console();
    
	pio_configure(PIOC, PIO_OUTPUT_1, 1 << 8, PIO_DEFAULT);
	
	/* Output example information. */
	puts(STRING_HEADER);
	
	h4d69_init();
	h4d69_enable_interrupt();
	
	while (1) {
		h4d69_update();
		if(h4d69_has_beat()) {
			pio_clear(PIOC, 1 << 8);
		} else {
			pio_set(PIOC, 1 << 8);
		}
		h4d69_convert();
		delay_ms(10);
	}
}
