/**
 * Março - 2020
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 * Marco Mello - macoasma@insper.edu.br
 *
 * Configura o ADC do SAME70 para fazer leitura de
 * um potenciômetro através do pino PD30
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */


/************************************************************************/
/* includes                                                             */
/************************************************************************/
#include <asf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "conf_board.h"
#include "conf_uart_serial.h"


/************************************************************************/
/* Defines                                                              */
/************************************************************************/

#define AFEC_CHANNEL_POT 0 // Canal do sensor de temperatura

/************************************************************************/
/* Globals                                                              */
/************************************************************************/

/************************************************************************/
/* Callbacks: / Handler                                                 */
/************************************************************************/

/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

static void config_ADC_TEMP(void){
/*************************************
   * Ativa e configura AFEC
   *************************************/
  /* Ativa AFEC - 0 */
	afec_enable(AFEC0);

	/* struct de configuracao do AFEC */
	struct afec_config afec_cfg;

	/* Carrega parametros padrao */
	afec_get_config_defaults(&afec_cfg);

	/* Configura AFEC */
	afec_init(AFEC0, &afec_cfg);

	/* Configura trigger por software */
	afec_set_trigger(AFEC0, AFEC_TRIG_SW);

	/*** Configuracao específica do canal AFEC ***/
	struct afec_ch_config afec_ch_cfg;
	afec_ch_get_config_defaults(&afec_ch_cfg);
	afec_ch_cfg.gain = AFEC_GAINVALUE_0;
	afec_ch_set_config(AFEC0, AFEC_CHANNEL_POT, &afec_ch_cfg);

	/*
	* Calibracao:
	* Because the internal ADC offset is 0x200, it should cancel it and shift
	 down to 0.
	 */
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_POT, 0x200);

	/***  Configura sensor de temperatura ***/
	struct afec_temp_sensor_config afec_temp_sensor_cfg;

	afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
	afec_temp_sensor_set_config(AFEC0, &afec_temp_sensor_cfg);

	/* Selecina canal e inicializa conversão */
	afec_channel_enable(AFEC0, AFEC_CHANNEL_POT);
}



/************************************************************************/
/* Main                                                                 */
/************************************************************************/

int main(void)
{
	/* Initialize the USART configuration struct */
	const usart_serial_options_t usart_serial_options = {
		.baudrate     = CONF_UART_BAUDRATE,
		.charlength   = CONF_UART_CHAR_LENGTH,
		.paritytype   = CONF_UART_PARITY,
		.stopbits     = CONF_UART_STOP_BITS
	};
	

	/* Initialize the SAM system. */
	sysclk_init(); /* Initialize system clocks */
	board_init();  /* Initialize board */
	
	
	/* Initialize stdio on USART */
	stdio_serial_init(CONF_UART, &usart_serial_options);


	/* inicializa e configura adc */
	config_ADC_TEMP();
  
  
  while(1){
	   afec_start_software_conversion(AFEC0); // Incializa conversão ADC
	   delay_ms(500); // Aguarda 500ms
	   
	   int valor = afec_channel_get_value(AFEC0, AFEC_CHANNEL_POT); // Pega o valor convertido e guarda na variável valor
	   printf("%d\n", valor); // Printa o valor na Serial (Range: 0 a 4096 ~ 0V a 3.3V)
  }

	return 0;
}
