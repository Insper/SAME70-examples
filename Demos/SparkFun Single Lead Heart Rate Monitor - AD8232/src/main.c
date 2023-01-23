/************************************************************************/
/* includes                                                             */
/************************************************************************/
#include <asf.h>
#include <string.h>
#include "ili9341.h"
#include "lvgl.h"
#include "touch/touch.h"
#include "conf_board.h"

/************************************************************************/
/* LEITURA DIGITAL/ANAL�GICA                                            */
/************************************************************************/
#define AFEC_POT AFEC1
#define AFEC_POT_ID ID_AFEC1
#define AFEC_POT_CHANNEL 6 // Canal do pino PC31

#define LO_MINUS_PIO PIOC
#define LO_MINUS_PIO_ID ID_PIOC
#define LO_MINUS_IDX 17
#define LO_MINUS_IDX_MASK (1u << LO_MINUS_IDX)

#define LO_PLUS_PIO PIOA
#define LO_PLUS_PIO_ID ID_PIOA
#define LO_PLUS_IDX 4
#define LO_PLUS_IDX_MASK (1u << LO_PLUS_IDX)

/************************************************************************/
/* Variáveis globais de uso do RTOS                                     */
/************************************************************************/

TimerHandle_t xTimer;
QueueHandle_t xQueueADC;
QueueHandle_t xQueueECG;

typedef struct {
	uint value;
} adcData;

/************************************************************************/
/* prototypes local                                                     */
/************************************************************************/
static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel,
afec_callback_t callback);
static void configure_console(void);
void io_init(void);

/************************************************************************/
/* LCD / LVGL                                                           */
/************************************************************************/

#define LV_HOR_RES_MAX          (320)
#define LV_VER_RES_MAX          (240)

/*A static or global variable to store the buffers*/
static lv_disp_draw_buf_t disp_buf;

/*Static or global buffer(s). The second buffer is optional*/
static lv_color_t buf_1[LV_HOR_RES_MAX * LV_VER_RES_MAX];
static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/
static lv_indev_drv_t indev_drv;

/*Vetor de alocação dos pontos a serem mostrados pelo ECG*/
#define CHAR_DATA_LEN 250
int ser1_data[CHAR_DATA_LEN];
lv_obj_t * chart;
lv_chart_series_t * ser1;

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_LCD_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY            (tskIDLE_PRIORITY)

#define TASK_ADC_STACK_SIZE (4096/ sizeof(portSTACK_TYPE))
#define TASK_ADC_STACK_PRIORITY (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) {  pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);}

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

void vTimerCallback(TimerHandle_t xTimer) {
	/* Selecina canal e inicializa convers�o */
	afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
	afec_start_software_conversion(AFEC_POT);
}

static void AFEC_pot_callback(void) {
	adcData adc;
	adc.value = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xQueueSendFromISR(xQueueADC, &adc, &xHigherPriorityTaskWoken);
}
/************************************************************************/
/* lvgl                                                                 */
/************************************************************************/

/* Tela de amostragem do ECG                                             */
void lv_screen_chart(void) {
	chart = lv_chart_create(lv_scr_act());
	lv_obj_set_size(chart, 300, 200);
	lv_obj_align(chart, LV_ALIGN_CENTER, 0, 0);
	lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
	lv_chart_set_range(chart,LV_CHART_AXIS_PRIMARY_Y, 100, 4095);
	lv_chart_set_point_count(chart, CHAR_DATA_LEN);
	lv_chart_set_div_line_count(chart, 0, 0);
	lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_SHIFT);

	ser1 = lv_chart_add_series(chart,  lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);
	lv_chart_set_ext_y_array(chart, ser1, (lv_coord_t *)ser1_data);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/


static void task_adc(void *pvParameters) {
  // Inicia pinos digitais
  io_init();
  // configura ADC e TC para controlar a leitura
  config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_callback);
  // Configura timer para captar pontos a cada 250Hz, permitindo efetividade na captação de pulsos
  xTimer = xTimerCreate("Timer",
                        250,       // amostragem escolhida         
                        pdTRUE,                      
                        (void *)0,
                        vTimerCallback);
  xTimerStart(xTimer, 0);

  // vari�vel para recever dados da fila
  adcData adc;

  while (1) {
	//Detecta se os pulsos identificados est�o na threshold do fabricante (filtro passa altas), 
	// se recebe algo nesse sinal digital, não deve exibir o pulso. Segundo o fabricante:
	/* The AD8232 includes a fast restore function that reduces the
	duration of otherwise long settling tails of the high-pass filters.
	After an abrupt signal change that rails the amplifier (such as a
	leads off condition), the AD8232 automatically adjusts to a
	higher filter cutoff. This feature allows the AD8232 to recover
	quickly, and therefore, to take valid measurements soon after
	connecting the electrodes to the subject.                                                                */
	if (pio_get(LO_MINUS_PIO,PIO_INPUT,LO_MINUS_IDX_MASK) || pio_get(LO_PLUS_PIO,PIO_INPUT,LO_PLUS_IDX_MASK)){ 
		   printf("-\n");
	}
	// Se dentro do Lead off, realiza a leitura do valor recebido na fila do analógico, printa seu valor
	// e envia para a task de gerenciamento gráfico da tela do ECG
     else if(xQueueReceive(xQueueADC, &(adc), 1000)) {
      	printf("ADC: %d \n", adc);
	  	xQueueSend(xQueueECG,&adc.value,0);
    }
  }
}

static void task_lcd(void *pvParameters) {
	int px, py;

	lv_screen_chart();
	int ecg;
	for (;;)  {
		// Recebe valor análogico adequado, preenche novo ponto e gera refresh na tela
		if (xQueueReceive(xQueueECG,&ecg,0)){
			lv_chart_set_next_value(chart, ser1, ecg);
			lv_chart_refresh(chart);
		}
		lv_tick_inc(50);
		lv_task_handler();
		vTaskDelay(50);
	}
}

/************************************************************************/
/* configs                                                              */
/************************************************************************/
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
  //afec_init(afec, &afec_cfg);

  /* Configura trigger por software */
  afec_set_trigger(afec, AFEC_TRIG_SW);

  /*** Configuracao espec�fica do canal AFEC ***/
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
  afec_set_callback(afec, afec_channel, callback, 0);
  NVIC_SetPriority(afec_id, 4);
  NVIC_EnableIRQ(afec_id);
}

void io_init(void) {
	// Configura-se leitura de output digital com Pull-up desligado
	pmc_enable_periph_clk(LO_MINUS_PIO_ID);
	pmc_enable_periph_clk(LO_PLUS_PIO_ID);
	
	pio_set_input(LO_MINUS_PIO, LO_MINUS_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(LO_MINUS_PIO, LO_MINUS_IDX_MASK, 0);
	
	pio_set_input(LO_PLUS_PIO, LO_PLUS_IDX_MASK, PIO_DEFAULT);
	pio_pull_up(LO_PLUS_PIO, LO_PLUS_IDX_MASK, 0);
}

static void configure_lcd(void) {
	/**LCD pin configure on SPI*/
	pio_configure_pin(LCD_SPI_MISO_PIO, LCD_SPI_MISO_FLAGS);  //
	pio_configure_pin(LCD_SPI_MOSI_PIO, LCD_SPI_MOSI_FLAGS);
	pio_configure_pin(LCD_SPI_SPCK_PIO, LCD_SPI_SPCK_FLAGS);
	pio_configure_pin(LCD_SPI_NPCS_PIO, LCD_SPI_NPCS_FLAGS);
	pio_configure_pin(LCD_SPI_RESET_PIO, LCD_SPI_RESET_FLAGS);
	pio_configure_pin(LCD_SPI_CDS_PIO, LCD_SPI_CDS_FLAGS);
	
	ili9341_init();
	ili9341_backlight_on();
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT,
	};

	/* Configure console UART. */
	stdio_serial_init(CONSOLE_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

/************************************************************************/
/* port lvgl                                                            */
/************************************************************************/

void my_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
	ili9341_set_top_left_limit(area->x1, area->y1);   ili9341_set_bottom_right_limit(area->x2, area->y2);
	ili9341_copy_pixels_to_screen(color_p,  (area->x2 + 1 - area->x1) * (area->y2 + 1 - area->y1));
	
	/* IMPORTANT!!!
	* Inform the graphics library that you are ready with the flushing*/
	lv_disp_flush_ready(disp_drv);
}

void my_input_read(lv_indev_drv_t * drv, lv_indev_data_t*data) {
	int px, py, pressed;
	
	if (readPoint(&px, &py))
		data->state = LV_INDEV_STATE_PRESSED;
	else
		data->state = LV_INDEV_STATE_RELEASED; 
	
	data->point.x = px;
	data->point.y = py;
}

void configure_lvgl(void) {
	lv_init();
	lv_disp_draw_buf_init(&disp_buf, buf_1, NULL, LV_HOR_RES_MAX * LV_VER_RES_MAX);
	
	lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
	disp_drv.draw_buf = &disp_buf;          /*Set an initialized buffer*/
	disp_drv.flush_cb = my_flush_cb;        /*Set a flush callback to draw to the display*/
	disp_drv.hor_res = LV_HOR_RES_MAX;      /*Set the horizontal resolution in pixels*/
	disp_drv.ver_res = LV_VER_RES_MAX;      /*Set the vertical resolution in pixels*/

	lv_disp_t * disp;
	disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/
	
	/* Init input on LVGL */
	lv_indev_drv_init(&indev_drv);
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	indev_drv.read_cb = my_input_read;
	lv_indev_t * my_indev = lv_indev_drv_register(&indev_drv);
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/
int main(void) {
	/* board and sys init */
	sysclk_init();
	board_init();
	configure_console();

	/* LCd, touch and lvgl init*/
	configure_lcd();
	configure_touch();
	configure_lvgl();
	
	xQueueADC = xQueueCreate(250, sizeof(adcData));
	xQueueECG =  xQueueCreate(32, sizeof(int));
	if (xQueueADC == NULL)
	printf("falha em criar a queue xQueueADC \n");

	/* Create task to control oled */
	if (xTaskCreate(task_adc, "adc", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create test ADC task\r\n");
	}
	
	if (xTaskCreate(task_lcd, "LCD", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create lcd task\r\n");
	}
	
	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){ }
}
