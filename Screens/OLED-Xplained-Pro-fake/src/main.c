#include <asf.h>
#include <string.h>
#include "conf_uart_serial.h"

static void configure_console(void){
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

void gfx_mono_draw_string(const char *str, uint32_t x,  uint32_t y){
  static char l1[32];
  static char l2[32];
  
  if(y==0)
    memcpy(l1,str,sizeof(l1));
  else
    memcpy(l2,str,sizeof(l2));
  
  if(l1[0] == 0)
    printf("\n");
  else
    printf(l1);
  if(l2[0] == 0)
    printf("\n");
  else
    printf(l2);
  printf("-------------------\n");
}

int main (void)
{
	board_init();
	sysclk_init();
	delay_init();
  configure_console();
   
  // escreve um texto na linha 0 do lcd
  gfx_mono_draw_string("linha1\n", 50, 0);
  
  // escreve um texto na linha 1 do lcd
  gfx_mono_draw_string("linha2\n", 50, 1);

  /* Insert application code here, after the board has been initialized. */
	while(1) {

	}
}
