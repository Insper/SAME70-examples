#include "asf.h"

#define BUT_PIO			  PIOA
#define BUT_PIO_ID		  ID_PIOA
#define BUT_PIO_IDX		  11
#define BUT_PIO_IDX_MASK (1u << BUT_PIO_IDX)

volatile int but_flag = 0;

void init(void);

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
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

void but_callback(void){
	but_flag = 1;
}

/*
Esse é o handler do TRNG que quando for chamado
realiza a criação de mais um número aleatório
*/
void TRNG_Handler(void)
{

		// Variável para sabermos se foi ou não acionado o handler
		uint32_t status;

		// Guardamos o valor desse status com essa função de interrupção
		status = trng_get_interrupt_status(TRNG);
	
		// Verificamos se foi ou nao acionado a chamada da função, e caso tenha sido chamada
		// Escrevemos no terminal a saída do output do TRNG que foi escolhido previamente
		if ((status & TRNG_ISR_DATRDY) == TRNG_ISR_DATRDY) {
			printf("-- Random Value: %lx --\n\r", trng_read_output_data(TRNG));
		}
}

void init(void){
	pmc_enable_periph_clk(BUT_PIO_ID);
	pio_configure(BUT_PIO, PIO_INPUT, BUT_PIO_IDX_MASK,
	PIO_PULLUP | PIO_DEBOUNCE);
	pio_handler_set(BUT_PIO, BUT_PIO_ID, BUT_PIO_IDX_MASK, PIO_IT_EDGE,
	but_callback);
	pio_enable_interrupt(BUT_PIO, BUT_PIO_IDX_MASK);
	pio_get_interrupt_status(BUT_PIO);
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, 4);
}

//parte principal do código que é rodado ao iniciar a placa
int main(void)
{
	sysclk_init();
	board_init();
	init();

	configure_console();
	
	printf("-- TRNG Example --\n\r");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\r\n", __DATE__, __TIME__);

	pmc_enable_periph_clk(ID_TRNG);

	trng_enable(TRNG);

	NVIC_DisableIRQ(TRNG_IRQn);
	NVIC_ClearPendingIRQ(TRNG_IRQn);
	NVIC_SetPriority(TRNG_IRQn, 0);
	NVIC_EnableIRQ(TRNG_IRQn);
	trng_enable_interrupt(TRNG);

	while (1) {
		
	}
}
