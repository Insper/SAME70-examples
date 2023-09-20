/************************************************************************
* 5 semestre - Eng. da Computao - Insper
*
* 2022 - Exemplo de fita de LEDs endereçável com RTOS
*
* Por: Renato Laffranchi Falcão :)
*/

/*
Dicas para a fita de LEDs:

1. Tem que usar uma fonte, a placa não consegue fornecer a corrente necessária para acender toda uma fita de LEDs;
2. Lembre de conectar o GND da fita ao GND da placa e ao GND da fonte;

*/

#include <asf.h>
#include "conf_board.h"
#include "helpers/helpers.h"
#include "ledHelpers/dataTypes.h"
#include "ledHelpers/ledColors.h"
/* 
* Os defines necessários para este exemplo estão definidos aqui em main.c
* Mas o resto dos defines, caso deseje incluílos, estão em defines.h
*/

//#include "helpers/defines.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

//LED DATA PIN - PD30
#define PIN_DATA      PIOD
#define PIN_DATA_ID   ID_PIOD
#define PIN_DATA_IDX  30
#define PIN_DATA_IDX_MASK (1 << PIN_DATA_IDX)

//MAIN BUT - Botão embutido da placa
#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_PIO_PIN 11
#define BUT_IDX_MASK (1 << BUT_PIO_PIN)

// Número de LEDs na fita
#define LEDS_NUMBER 44

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_LED_STACK_SIZE (4096*8 / sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY        (tskIDLE_PRIORITY)

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/************************************************************************/
/* recursos RTOS                                                        */
/************************************************************************/

QueueHandle_t xQueueLed;

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes local                                                     */
/************************************************************************/

static void LED_init(void);
void LEDS_light_up(char *, int);
void but_callback(void);
void but_init(void);
static void configure_console(void);

/************************************************************************/
/* RTOS application HOOK                                                */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

void but_callback(void) {
	// Cria um array leds cheio de 1s
	char leds[LEDS_NUMBER];
	for (int i = 0; i < LEDS_NUMBER; i++) {
		leds[i] = 1;
	}
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xQueueSendFromISR(xQueueLed, leds, &xHigherPriorityTaskWoken);
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_led(void *pvParameters) {
	char leds[LEDS_NUMBER];
	for(;;){
		// Acende LEDs apenas se o botão for pressionado
		if (xQueueReceive(xQueueLed, leds, 100)) {
			taskENTER_CRITICAL();
			LEDS_light_up(leds, LEDS_NUMBER);
			taskEXIT_CRITICAL();
			vTaskDelay(100);
			clearLEDs();
		}
	}
}

/************************************************************************/
/* configs                                                              */
/************************************************************************/

void LED_init(void) {
	pmc_enable_periph_clk(PIN_DATA_ID);
	pio_configure(PIN_DATA, PIO_OUTPUT_0, PIN_DATA_IDX_MASK, PIO_DEFAULT);
}

void but_init(void) {	
	config_button(BUT_PIO, BUT_IDX_MASK, BUT_PIO_ID, but_callback, 1, 1);
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

/************************************************************************/
/* funcs                                                              */
/************************************************************************/

void LEDS_light_up(char *array, int n) {
	/* Acender todos os leds com apenas uma cor*/
	//for(int i = 0; i < n; i++) {
		 //colorGREEN();
	//}
	
	/* Se quiser acender só alguns LEDS específicos */
	//for(int i = 0; i < n; i++) array[i] ? colorRED() : colorBLACK(); 
	
	/* Se quiser acender todos os leds com cada uma das 3 cores */
	for(int i = 0; i < n; i++) {
		colorGREEN();
	}
	delay_ms(500);
	for(int j = 0; j < n; j++) {
		colorBLUE();
	}
	delay_ms(500);
	for(int k = 0; k < n; k++) {
		colorRED();
	}
	
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/

int main(void) {
	/* Initialize the SAM system */
	sysclk_init();
	board_init();
	
	LED_init();
	but_init();
	
	/* Initialize the console uart */
	configure_console();	

	/* Create task to make led blink */
	xQueueLed = xQueueCreate(20, sizeof(char));
	if (xQueueLed == NULL){
		printf("Falhou ao criar a xQueueLed \n");
	}
	
	if (xTaskCreate(task_led, "LED", TASK_LED_STACK_SIZE, NULL, TASK_LED_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Falhou ao criar a task_led \r\n");
	}

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}