/**
* Rafael Corsi @ insper . edu . br
* 2021
* Exemplo de uso do LCD 2.8" da adafruit
* com controlador ili9341 com o kit same70-xpld
*
* neste exemplo a imagem foi convertida via
* o programa imageconverter.
*
* 1. Configura o LCD
* 1. Desenha a imagem no LCD
*
* Para conexão elétrica acesse o README.md
*
**/
#include <asf.h>
#include "ili9341.h"
#include "touch.h"

typedef struct {
  const uint16_t *data;
  uint16_t width;
  uint16_t height;
  uint8_t dataSize;
} tImage;
#include "Image.h"

/** Display background color when clearing the display */
#define BG_COLOR  ILI9341_COLOR(255, 0, 255)

/** RTOS  */
#define TASK_LCD_STACK_SIZE                (1024*10/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY            (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/************************************************************************/
/* RTOS application funcs                                               */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
  printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
  for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) { configASSERT( ( volatile void * ) NULL ); }

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

static void configure_console(void) {
  const usart_serial_options_t uart_serial_options = {
    .baudrate     = USART_SERIAL_EXAMPLE_BAUDRATE,
    .charlength   = USART_SERIAL_CHAR_LENGTH,
    .paritytype   = USART_SERIAL_PARITY,
    .stopbits     = USART_SERIAL_STOP_BIT
  };

  /* Configure console UART. */
  stdio_serial_init(USART_SERIAL_EXAMPLE, &uart_serial_options);

  /* Specify that stdout should not be buffered. */
  setbuf(stdout, NULL);
}

static void configure_lcd_pins(void) {
  /**LCD pin configure on SPI*/
  pio_configure_pin(LCD_SPI_MISO_PIO, LCD_SPI_MISO_FLAGS);
  pio_configure_pin(LCD_SPI_MOSI_PIO, LCD_SPI_MOSI_FLAGS);
  pio_configure_pin(LCD_SPI_SPCK_PIO, LCD_SPI_SPCK_FLAGS);
  pio_configure_pin(LCD_SPI_NPCS_PIO, LCD_SPI_NPCS_FLAGS);
  pio_configure_pin(LCD_SPI_RESET_PIO, LCD_SPI_RESET_FLAGS);
  pio_configure_pin(LCD_SPI_CDS_PIO, LCD_SPI_CDS_FLAGS);
}

/************************************************************************/
/* tasks                                                                */
/************************************************************************/
void task_lcd(void){
  /* initialize ios and the display controller */
  configure_lcd_pins();
  ili9341_init();
  
  /* init touch */
  configure_touch();
  
  int px, py;

  /* set x,y pointer on screen */
  /* set update range  */
  ili9341_set_top_left_limit(0, 0);
  ili9341_set_bottom_right_limit(Image.width, Image.height);

  /* send pxs to screen */
  ili9341_copy_pixels_to_screen(Image.data, ILI9341_DEFAULT_WIDTH*ILI9341_DEFAULT_HEIGHT);
  
  while(1) {
    if(readPoint(&px, &py)) {
      printf("%03d, %03d \n", px, py);
    }
    vTaskDelay(100);
  }
}


/************************************************************************/
/* main                                                                 */
/************************************************************************/
int main(void) {
  board_init();
  sysclk_init();
  configure_console();

  /* Create task to control oled */
  if (xTaskCreate(task_lcd, "lcd", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
    printf("Failed to create oled task\r\n");
  }
  
  /* Start the scheduler. */
  vTaskStartScheduler();

  /* RTOS n�o deve chegar aqui !! */
  while(1){}


}
