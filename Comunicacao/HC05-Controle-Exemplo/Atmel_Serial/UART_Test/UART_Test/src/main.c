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


volatile long g_systimer = 0;

void SysTick_Handler() {
	g_systimer++;
}


void config_console(void) {
	usart_serial_options_t config;
	config.baudrate = 9600;
	config.charlength = US_MR_CHRL_8_BIT;
	config.paritytype = US_MR_PAR_NO;
	config.stopbits = false;
	usart_serial_init(USART1, &config);
	usart_enable_tx(USART1);
	usart_enable_rx(USART1);
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();
	SysTick_Config(sysclk_get_cpu_hz() / 1000); // 1 ms
	config_console();
	char button1 = '0';
	char eof = 'X';
	char buffer[1024];
	
	while(1) {
		if(pio_get(PIOA, PIO_INPUT, PIO_PA11) == 0) {
			button1 = '1';
		} else {
			button1 = '0';
		}
		
		while(!usart_is_tx_ready(USART1));
		usart_write(USART1, button1);
		while(!usart_is_tx_ready(USART1));
		usart_write(USART1, eof);
	}
}
