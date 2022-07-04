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

#include "conf_board.h"
#include "oled/gfx_mono_ug_2832hsweg04.h"
#include "oled/gfx_mono_text.h"
#include "oled/sysfont.h"

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

/** RTOS  */
#define TASK_OLED_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_OLED_STACK_PRIORITY            (tskIDLE_PRIORITY)

#define TASK_ADC_STACK_SIZE (1024*10 / sizeof(portSTACK_TYPE))
#define TASK_ADC_STACK_PRIORITY (tskIDLE_PRIORITY)

#define AFEC_POT AFEC0
#define AFEC_POT_ID ID_AFEC0
#define AFEC_POT_CHANNEL 0 // Canal do pino PD30

/** Queue for msg log send data */
QueueHandle_t xQueueADC;

typedef struct {
	uint value;
} adcData;

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
/* prototypes local                                                     */
/************************************************************************/


void TC_init(Tc *TC, int ID_TC, int TC_CHANNEL, int freq);
static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback);

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void TC1_Handler(void) {
	volatile uint32_t ul_dummy;

	ul_dummy = tc_get_status(TC0, 1);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/* Selecina canal e inicializa conversão */
	afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
	afec_start_software_conversion(AFEC_POT);
}
static void AFEC_pot_Callback(void) {
	adcData adc;
	adc.value = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xQueueSendFromISR(xQueueADC, &adc, &xHigherPriorityTaskWoken);
}

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

static void task_adc(void *pvParameters) {

	// configura ADC e TC para controlar a leitura
	config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_Callback);
	TC_init(TC0, ID_TC1, 1, 10);
	tc_start(TC0, 1);

	// variável para recever dados da fila
	adcData adc;

	while (1) {
		
	}
}

static void task_oled(void *pvParameters) {
	gfx_mono_ssd1306_init();
	gfx_mono_draw_string("Teste SAMe70-XPLD", 0, 0, &sysfont);
	gfx_mono_draw_string("oi", 0, 20, &sysfont);
	char buffer[4];
	char cont;
	 adcData adc;
	 
	for (;;)  {
		
		
		if (xQueueReceive(xQueueADC, &(adc), 1)) {
			sprintf(buffer, "%4d", adc);
			gfx_mono_draw_string(buffer, 0, 20, &sysfont);
			printf("ADC: %d \n", adc);
			
			} 
		vTaskDelay(100);
		
	}
}


void task_lcd(void){
  /* initialize ios and the display controller */
  configure_lcd_pins();
  ili9341_init();
  
  ili9341_set_orientation(ILI9341_FLIP_X);
  
  /* init touch */
  configure_touch();
  
  int px, py;

  /* set x,y pointer on screen */
  /* set update range  */
  ili9341_set_top_left_limit(0, 0);
  ili9341_set_bottom_right_limit(Image.width, Image.height);

  /* send pxs to screen */
  ili9341_copy_pixels_to_screen(Image.data, ILI9341_DEFAULT_WIDTH*ILI9341_DEFAULT_HEIGHT);
  
   /* Create task to control oled */
   if (xTaskCreate(task_oled, "oled", TASK_OLED_STACK_SIZE, NULL, TASK_OLED_STACK_PRIORITY, NULL) != pdPASS) {
	   printf("Failed to create oled task\r\n");
   }
  
  while(1) {
    if(readPoint(&px, &py)) {
  //    printf("%03d, %03d \n", px, py);
    }
    vTaskDelay(100);
  }
}


static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel,
                            afec_callback_t callback) {
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
  afec_set_callback(afec, afec_channel, callback, 1);
  NVIC_SetPriority(afec_id, 4);
  NVIC_EnableIRQ(afec_id);
}

void TC_init(Tc *TC, int ID_TC, int TC_CHANNEL, int freq) {
  uint32_t ul_div;
  uint32_t ul_tcclks;
  uint32_t ul_sysclk = sysclk_get_cpu_hz();

  pmc_enable_periph_clk(ID_TC);

  tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
  tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
  tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

  NVIC_SetPriority((IRQn_Type)ID_TC, 4);
  NVIC_EnableIRQ((IRQn_Type)ID_TC);
  tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/
int main(void) {
  board_init();
  sysclk_init();
  configure_console();
  
  xQueueADC = xQueueCreate(1, sizeof(adcData));
  if (xQueueADC == NULL)
  printf("falha em criar a queue xQueueADC \n");

  if (xTaskCreate(task_adc, "ADC", TASK_ADC_STACK_SIZE, NULL,
  TASK_ADC_STACK_PRIORITY, NULL) != pdPASS) {
	  printf("Failed to create test ADC task\r\n");
  }

  /* Create task to control oled */
  if (xTaskCreate(task_lcd, "lcd", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
    printf("Failed to create oled task\r\n");
  }
  
 
  
  /* Start the scheduler. */
  vTaskStartScheduler();

  /* RTOS n�o deve chegar aqui !! */
  while(1){}


}
