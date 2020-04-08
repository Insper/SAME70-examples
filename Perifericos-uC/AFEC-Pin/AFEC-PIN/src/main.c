/**
* Março - 2020
* 5 semestre - Eng. da Computação - Insper
* Rafael Corsi - rafael.corsi@insper.edu.br
* Marco Mello - macoasma@insper.edu.br
*
* Configura o ADC do SAME70 para fazer leitura de
* um potenciômetro através do pino PC31
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

#define AFEC_POT AFEC1
#define AFEC_POT_ID ID_AFEC1
#define AFEC_POT_CHANNEL 6 // Canal do pino PC31

/************************************************************************/
/* Globals                                                              */
/************************************************************************/

/** The conversion data is done flag */
volatile bool g_is_conversion_done = false;

/** The conversion data value */
volatile uint32_t g_ul_value = 0;

/************************************************************************/
/* Callbacks: / Handler                                                 */
/************************************************************************/

/**
* \brief AFEC interrupt callback function.
*/
static void AFEC_pot_Callback(void){
  g_ul_value = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
  g_is_conversion_done = true;
}

/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback){
  /*************************************
  * Ativa e configura AFEC
  *************************************/
  /* Ativa AFEC - 0 */
  afec_enable(afec);

  /* struct de configuracao do AFEC */
  struct afec_config afec_cfg;

  /* Carrega parametros padrao */
  afec_get_config_defaults(&afec_cfg);

  /* Configura AFEC */
  afec_init(afec, &afec_cfg);

  /* Configura trigger por software */
  afec_set_trigger(afec, AFEC_TRIG_SW);

  /*** Configuracao específica do canal AFEC ***/
  struct afec_ch_config afec_ch_cfg;
  afec_ch_get_config_defaults(&afec_ch_cfg);
  afec_ch_cfg.gain = AFEC_GAINVALUE_0;
  afec_ch_set_config(afec, afec_channel, &afec_ch_cfg);

  /*
  * Calibracao:
  * Because the internal ADC offset is 0x200, it should cancel it and shift
  down to 0.
  */
  afec_channel_set_analog_offset(afec, afec_channel, 0x200);

  /***  Configura sensor de temperatura ***/
  struct afec_temp_sensor_config afec_temp_sensor_cfg;

  afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
  afec_temp_sensor_set_config(afec, &afec_temp_sensor_cfg);
  
  /* configura IRQ */
  afec_set_callback(afec, afec_channel,	callback, 1);
  NVIC_SetPriority(afec_id, 4);
  NVIC_EnableIRQ(afec_id);
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
  config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_Callback);
  
  /* Selecina canal e inicializa conversão */
  afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
  afec_start_software_conversion(AFEC_POT);
  
  while(1){
    if(g_is_conversion_done){
      printf("%d\n", g_ul_value);               
      delay_ms(500);                          
      
      /* Selecina canal e inicializa conversão */
      afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
      afec_start_software_conversion(AFEC_POT);
    }
  }

  return 0;
}
