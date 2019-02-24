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
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>


/** The conversion data is done flag */
volatile bool is_conversion_done = false;

/** The conversion data value */
volatile uint32_t g_ul_value = 0;

#define LM35DZ_AFEC         AFEC0
#define LM35DZ_AFEC_ID      ID_AFEC0
#define LM35DZ_AFEC_CH      AFEC_CHANNEL_5 // Pin PB2
#define LM35DZ_AFEC_CH_IR   AFEC_INTERRUPT_EOC_5

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


static void afec_temp_sensor_end_conversion(void)
{
	g_ul_value = afec_channel_get_value(LM35DZ_AFEC, LM35DZ_AFEC_CH);
	is_conversion_done = true;
}

void lm35dz_init(void) {
	pmc_enable_periph_clk(LM35DZ_AFEC_ID);
	afec_enable(LM35DZ_AFEC);
	struct afec_config afec_cfg;
	afec_get_config_defaults(&afec_cfg);
	afec_init(LM35DZ_AFEC, &afec_cfg);
	afec_set_trigger(LM35DZ_AFEC, AFEC_TRIG_SW);

	struct afec_ch_config afec_ch_cfg;
	afec_ch_get_config_defaults(&afec_ch_cfg);
	afec_ch_cfg.gain = AFEC_GAINVALUE_0;
	afec_ch_set_config(LM35DZ_AFEC, LM35DZ_AFEC_CH, &afec_ch_cfg);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_5, 0x200); // internal ADC offset is 0x200, it should cancel it and shift to 0
	afec_set_callback(LM35DZ_AFEC, LM35DZ_AFEC_CH_IR, afec_temp_sensor_end_conversion, 1);
	afec_channel_enable(LM35DZ_AFEC, LM35DZ_AFEC_CH);
	NVIC_SetPriority(AFEC0_IRQn, 10);
	
}

void lm35dz_enable_interrupt(void) {
	afec_enable_interrupt(LM35DZ_AFEC, LM35DZ_AFEC_CH_IR);
	NVIC_EnableIRQ(AFEC0_IRQn);
}

void lm35dz_convert(void) {
	if(!is_conversion_done) {
		afec_start_software_conversion(AFEC0);
	}
}

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */

	board_init();
	delay_init();
	sysclk_init();
	lm35dz_init();
	configure_console();
	lm35dz_enable_interrupt();
	puts("TESTE LM35DZ\r\n");
	while(1) {
		lm35dz_convert();
		//delay_ms(1);
		if(is_conversion_done){
			uint32_t g_voltage = g_ul_value * 3300 / 4096; // in mv
			printf("temp 0 : %d\r\n", g_voltage/10);
			is_conversion_done = false;
		}
		
		
	}
	/* Insert application code here, after the board has been initialized. */
}
