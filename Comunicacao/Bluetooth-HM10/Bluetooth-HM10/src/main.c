#include "asf.h"

/************************************************************************/
/* Pinos                                                                */
/************************************************************************/

/**
 * LEDs
 */
#define LED_PIO_ID		  ID_PIOC
#define LED_PIO         PIOC
#define LED_PIN		      8
#define LED_PIN_MASK    (1<<LED_PIN)

/**
 * Botão
 */
#define BUT_PIO_ID            ID_PIOA
#define BUT_PIO               PIOA
#define BUT_PIN		            11
#define BUT_PIN_MASK          (1 << BUT_PIN)
#define BUT_DEBOUNCING_VALUE  79

/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/

volatile uint8_t flag_led0 = 1;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void BUT_init(void);
void LED_init(int estado);
void pin_toggle(Pio *pio, uint32_t mask);
uint32_t usart0_putString(uint8_t *pstring);
uint32_t usart0_getString(uint8_t *pstring);

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

static void Button1_Handler(uint32_t id, uint32_t mask) {
  pin_toggle(PIOD, (1<<28));
  pin_toggle(LED_PIO, LED_PIN_MASK);
}

/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

/**
 *  Toggle pin controlado pelo PIO (out)
 */
void pin_toggle(Pio *pio, uint32_t mask) {
   if(pio_get_output_data_status(pio, mask))
    pio_clear(pio, mask);
   else
    pio_set(pio,mask);
}

/**
 * Inicializa o pino do BUT
 */
void BUT_init(void) {
    /* config. pino botao em modo de entrada */
    pmc_enable_periph_clk(BUT_PIO_ID);
    pio_set_input(BUT_PIO, BUT_PIN_MASK, PIO_PULLUP | PIO_DEBOUNCE);

    /* config. interrupcao em borda de descida no botao do kit */
    /* indica funcao (but_Handler) a ser chamada quando houver uma interrupção */
    pio_enable_interrupt(BUT_PIO, BUT_PIN_MASK);
    pio_handler_set(BUT_PIO, BUT_PIO_ID, BUT_PIN_MASK, PIO_IT_FALL_EDGE, Button1_Handler);

    /* habilita interrupçcão do PIO que controla o botao */
    /* e configura sua prioridade                        */
    NVIC_EnableIRQ(BUT_PIO_ID);
    NVIC_SetPriority(BUT_PIO_ID, 1);
};

/**
 * Inicializa o pino do LED
 */
void LED_init(int estado) {
    pmc_enable_periph_clk(LED_PIO_ID);
    pio_set_output(LED_PIO, LED_PIN_MASK, estado, 0, 0 );
};

/**
 * Configure UART console.
 * BaudRate : 115200
 * 8 bits
 * 1 stop bit
 * sem paridade
 */
static void configure_console(void) {

  /* Configura USART1 Pinos */
  sysclk_enable_peripheral_clock(ID_PIOB);
  sysclk_enable_peripheral_clock(ID_PIOA);
  pio_set_peripheral(PIOB, PIO_PERIPH_D, PIO_PB4);  // RX
  pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA21); // TX
 	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;

	const usart_serial_options_t uart_serial_options = {
		.baudrate   = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits   = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/**
 * Configure UART Bluetooth.
 */
static void USART0_init(void) {

  /* Configura USART0 Pinos */
  sysclk_enable_peripheral_clock(ID_PIOB);
  pio_set_peripheral(PIOB, PIO_PERIPH_C, PIO_PB0);
  pio_set_peripheral(PIOB, PIO_PERIPH_C, PIO_PB1);

  /* Configura opcoes USART */
  const sam_usart_opt_t usart_settings = {
    .baudrate     = 9600,
    .char_length  = US_MR_CHRL_8_BIT,
    .parity_type  = US_MR_PAR_NO,
    .stop_bits    = US_MR_NBSTOP_1_BIT,
    .channel_mode = US_MR_CHMODE_NORMAL
  };

  /* Ativa Clock periferico USART0 */
  sysclk_enable_peripheral_clock(ID_USART0);

  /* Configura USART para operar em modo RS232 */
  usart_init_rs232(USART0, &usart_settings, sysclk_get_peripheral_hz());

  /* Enable the receiver and transmitter. */
	usart_enable_tx(USART0);
	usart_enable_rx(USART0);
 }

/**
 *  Envia uma string para USART0
 */
uint32_t usart0_putString(uint8_t *pstring) {
  uint32_t i = 0 ;

  while(*(pstring + i)){
    usart_serial_putchar(USART0, *(pstring+i++));
    while(!uart_is_tx_empty(USART0)){};
  }

  return(i);
}

/**
 * Busca uma string da USART0
 */
uint32_t usart0_getString(uint8_t *pstring) {
  uint32_t i = 0 ;

  usart_serial_getchar(USART0, (pstring+i));
  while(*(pstring+i) != '\n'){
    usart_serial_getchar(USART0, (pstring+(++i)));
  }
  *(pstring+i+1)= 0x00;
  return(i);

}

/************************************************************************/
/* Main Code	                                                        */
/************************************************************************/
int main(void){

  /* buffer para recebimento de dados */
  uint8_t bufferRX[100];
  uint8_t bufferTX[100];

  /* Initialize the SAM system */
  sysclk_init();

  /* Disable the watchdog */
  WDT->WDT_MR = WDT_MR_WDDIS;

  /* Inicializa com serial com PC*/
  /* USART1 */
  configure_console();

  /* Configura Leds */
  LED_init(1);

  /* Configura os botões */
  BUT_init();

  /* Configura USART0 para comunicacao com o HM-10 */
  USART0_init();

  /* Inicializa funcao de delay */
  delay_init( sysclk_get_cpu_hz());

	while (1) {
    sprintf(bufferTX, "%s \n", "Ola Voce");
    printf("Olá Você \n");
    usart0_putString(bufferTX);
    usart0_getString(bufferRX);
    delay_s(1);
	}
}
