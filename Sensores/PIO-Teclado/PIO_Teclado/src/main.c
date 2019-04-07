/**
* \file
*
* \brief Empty user application template
*
*/

/**
* \mainpage User Application template doxygen documentation
*
* \par Empty user application template
*
* Bare minimum empty user application template
*
* \par Content
*
* -# Include the ASF header files (through asf.h)
* -# "Insert system clock initialization code here" comment
* -# Minimal main function that starts with a call to board_init()
* -# "Insert application code here" comment
*
*/

/*
* Include header files for all drivers that have been imported from
* Atmel Software Framework (ASF).
*/
/*
* Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
*/
#include <asf.h>

#define KEYPAD_LINE_1_PIO PIOC
#define KEYPAD_LINE_1 PIO_PC31

#define KEYPAD_LINE_2_PIO PIOB
#define KEYPAD_LINE_2 PIO_PB3

#define KEYPAD_LINE_3_PIO PIOA
#define KEYPAD_LINE_3 PIO_PA0

#define KEYPAD_LINE_4_PIO PIOD
#define KEYPAD_LINE_4 PIO_PD28

#define KEYPAD_COLUMN_1_PIO PIOA
#define KEYPAD_COLUMN_1 PIO_PA3
#define KEYPAD_COLUMN_2_PIO PIOB
#define KEYPAD_COLUMN_2 PIO_PB0
#define KEYPAD_COLUMN_3_PIO PIOD
#define KEYPAD_COLUMN_3 PIO_PD25
#define KEYPAD_COLUMN_4_PIO PIOD
#define KEYPAD_COLUMN_4 PIO_PD20


static void configure_keypad(void) {
	pmc_enable_periph_clk(ID_PIOA);
	pmc_enable_periph_clk(ID_PIOB);
	pmc_enable_periph_clk(ID_PIOC);
	pmc_enable_periph_clk(ID_PIOD);
	
	pio_configure(KEYPAD_LINE_1_PIO, PIO_OUTPUT_1, KEYPAD_LINE_1, PIO_DEFAULT);
	pio_configure(KEYPAD_COLUMN_1_PIO, PIO_INPUT, KEYPAD_COLUMN_1, PIO_PULLUP);

	pio_configure(KEYPAD_LINE_2_PIO, PIO_OUTPUT_1, KEYPAD_LINE_2, PIO_DEFAULT);
	pio_configure(KEYPAD_COLUMN_2_PIO, PIO_INPUT, KEYPAD_COLUMN_2, PIO_PULLUP);
	
	pio_configure(KEYPAD_LINE_3_PIO, PIO_OUTPUT_1, KEYPAD_LINE_3, PIO_DEFAULT);
	pio_configure(KEYPAD_COLUMN_3_PIO, PIO_INPUT, KEYPAD_COLUMN_3, PIO_PULLUP);
	
	pio_configure(KEYPAD_LINE_4_PIO, PIO_OUTPUT_1, KEYPAD_LINE_4, PIO_DEFAULT);
	pio_configure(KEYPAD_COLUMN_4_PIO, PIO_INPUT, KEYPAD_COLUMN_4, PIO_PULLUP);
	
}

static int keypad_check_pin(uint32_t line_pio, uint32_t line, uint32_t column_pio, uint32_t column, uint32_t debouce_in_ms) {
	pio_clear(line_pio, line);
	delay_ms(20);
	if(pio_get(column_pio, PIO_INPUT, column) == LOW) {
		delay_ms(debouce_in_ms);
		return pio_get(column_pio, PIO_INPUT, column) == LOW;
	}
	return 0;
}

static char keypad_get_value(uint32_t debouce_in_ms) {
	uint32_t KEYPAD_LINE_PIO_ARRAY[] = {KEYPAD_LINE_1_PIO, KEYPAD_LINE_2_PIO, KEYPAD_LINE_3_PIO, KEYPAD_LINE_4_PIO};
	uint32_t KEYPAD_LINE_ARRAY[] = {KEYPAD_LINE_1, KEYPAD_LINE_2, KEYPAD_LINE_3, KEYPAD_LINE_4};
	uint32_t KEYPAD_COLUMN_PIO_ARRAY[] = {KEYPAD_COLUMN_1_PIO, KEYPAD_COLUMN_2_PIO, KEYPAD_COLUMN_3_PIO, KEYPAD_COLUMN_4_PIO};
	uint32_t KEYPAD_COLUMN_ARRAY[] = {KEYPAD_COLUMN_1, KEYPAD_COLUMN_2, KEYPAD_COLUMN_3, KEYPAD_COLUMN_4};
	char KEYPAD_OUTPUT_ARRAY[] = {'1', '2', '3', 'A', '4', '5','6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D' };
	
	for(int i = 0; i < 4; i++) {
		for(int j = 0; j < 4; j++) {
			pio_set(KEYPAD_LINE_1_PIO, KEYPAD_LINE_1);
			pio_set(KEYPAD_LINE_2_PIO, KEYPAD_LINE_2);
			pio_set(KEYPAD_LINE_3_PIO, KEYPAD_LINE_3);
			pio_set(KEYPAD_LINE_4_PIO, KEYPAD_LINE_4);
			if(keypad_check_pin(KEYPAD_LINE_PIO_ARRAY[i], KEYPAD_LINE_ARRAY[i], KEYPAD_COLUMN_PIO_ARRAY[j],  KEYPAD_COLUMN_ARRAY[j],debouce_in_ms)) {
				return KEYPAD_OUTPUT_ARRAY[i*4+j];
			}
		}
	}
	
	return ' ';
}
// Console - UART
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate =		CONF_UART_BAUDRATE,
		.charlength =	CONF_UART_CHAR_LENGTH,
		.paritytype =	CONF_UART_PARITY,
		.stopbits =		CONF_UART_STOP_BITS,
	};

	/* Configure UART console. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
	usart_enable_rx(CONF_UART);
	usart_enable_tx(CONF_UART);
}

int main (void)
{

	board_init();
	sysclk_init();
	delay_init();
	configure_console();
	configure_keypad();
	while(1) {
		char key = keypad_get_value(50);
		if(key != ' ') {
			printf("%c", key);
			delay_ms(200);
		}
	}
}
