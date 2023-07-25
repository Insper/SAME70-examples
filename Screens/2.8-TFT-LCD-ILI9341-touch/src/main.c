
#include <asf.h>
#include <string.h>

#include "ili9341.h"
#include "touch.h"

int main(void)
{
  sysclk_init();
  board_init();
  
  /* Initialize the USART configuration struct */
  const usart_serial_options_t usart_serial_options = {
    .baudrate     = USART_SERIAL_EXAMPLE_BAUDRATE,
    .charlength   = USART_SERIAL_CHAR_LENGTH,
    .paritytype   = USART_SERIAL_PARITY,
    .stopbits     = USART_SERIAL_STOP_BIT
  };
  
  /* Initialize stdio on USART */
  stdio_serial_init(USART_SERIAL_EXAMPLE, &usart_serial_options);
  
  /* init touch */
  configure_touch();
  
  int px, py;
  while(1){
    
    if(readPoint(&px, &py)) {
      printf("%03d, %03d \n", px, py);
    }      
    delay_ms(100);
  }

}
