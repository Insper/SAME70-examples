#include <asf.h>
#include "conf_board.h"

/************************************************************************/
/* RTOS                                                                */
/************************************************************************/

/** RTOS  */
#define TASK_COUNTER_STACK_SIZE            (1024/sizeof(portSTACK_TYPE))
#define TASK_COUNTER_STACK_PRIORITY        (tskIDLE_PRIORITY)
#define TASK_UARTTX_STACK_SIZE             (2048/sizeof(portSTACK_TYPE))
#define TASK_UARTTX_STACK_PRIORITY         (tskIDLE_PRIORITY)
#define TASK_UARTRX_STACK_SIZE             (2048/sizeof(portSTACK_TYPE))
#define TASK_UARTRX_STACK_PRIORITY         (1)
#define TASK_PROCESS_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
#define TASK_PROCESS_STACK_PRIORITY        (2)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/** Semaforo a ser usado pela task led */
SemaphoreHandle_t xSemaphore;

/** Queue for msg log send data */
QueueHandle_t xQueueTx;

/** Queue for IRQ -> taskRx */
QueueHandle_t xQueueRx;
char ucMessageRx;

/** Queue for taskRx -> taskProcess */
QueueHandle_t xQueueMSG;
char ucMessageMSG[128];

/** prototypes */
void but_callback(void);
static void BUT_init(void);
static void USART1_init(void);
uint32_t usart_puts(uint8_t *pstring);

/************************************************************************/
/* RTOS application funcs                                               */
/************************************************************************/

/**
 * \brief Called if stack overflow during execution
 */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName)
{
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	 * identify which task has overflowed its stack.
	 */
	for (;;) {
	}
}

/**
 * \brief This function is called by FreeRTOS idle task
 */
extern void vApplicationIdleHook(void)
{
	pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
}

/**
 * \brief This function is called by FreeRTOS each tick
 */
extern void vApplicationTickHook(void)
{
}

extern void vApplicationMallocFailedHook(void)
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

void USART1_Handler(void){
	uint32_t ret = usart_get_status(USART_COM);

	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	char c;

	// Verifica por qual motivo entrou na interrupçcao
	//  - Dadodisponível para leitura
	if(ret & US_IER_RXRDY){
		usart_serial_getchar(USART_COM, &c);
		xQueueSendFromISR(xQueueTx, (void *) &c, &xHigherPriorityTaskWoken);

	// -  Transmissoa finalizada
	} else if(ret & US_IER_TXRDY){

	}
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_counter(void *pvParameters){
	if (xSemaphore == NULL)
		printf("falha em criar o semaforo \n");

	uint8_t cnt = 0 ;
  char ucMessageTx[128];

	for (;;) {
			sprintf(ucMessageTx, "cnt = %d \n", cnt++);
			LED_Toggle(TRIGGER0);
			xQueueSend(xQueueTx, (void *) &ucMessageTx, 10);
			vTaskDelay(1000);
	}
}

static void task_uartTx(void *pvParameters){

	xQueueTx = xQueueCreate(32, sizeof(ucMessageTx) );
	if(xQueueTx == NULL){
		printf("Fail to create xQueueTx \n");
		while(1){};
	}

	char rxMSG[128];
	while(1){
		if( xQueueReceive( xQueueTx, &rxMSG, ( TickType_t ) 1000 )){
			usart_puts(rxMSG);
		}
	}
}

static void task_uartRx(void *pvParameters){
	char rxMSG;
	char s[128];
	uint i = 0;

	xQueueRx = xQueueCreate(32, sizeof(ucMessageRx) );
	if(xQueueRx == NULL ){
		printf("Fail to create xQueueRx \n");
		while(1){};
	}

	while(1){
		if( xQueueReceive( xQueueRx, &rxMSG, ( TickType_t ) 500 )){
			if(rxMSG != '\n'){
				s[i++] = rxMSG;
			}
			else{
				xQueueSend(xQueueMSG, (void *) &s, 0);
			}
		}
	}
}

static void task_Process(void *pvParameters){
	char rxMSG[128];

	xQueueMSG = xQueueCreate(32, sizeof(ucMessageMSG) );
	if(xQueueMSG == NULL ){
		printf("Fail to create xQueueMSG \n");
		while(1){};
	}

	while(1){
		if( xQueueReceive(xQueueMSG, &rxMSG, ( TickType_t ) 2000 )){
			usart_puts(rxMSG);
		}
	}
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

static void USART1_init(void){
	/* Configura USART1 Pinos */
	sysclk_enable_peripheral_clock(ID_PIOB);
	sysclk_enable_peripheral_clock(ID_PIOA);
	pio_set_peripheral(PIOB, PIO_PERIPH_D, PIO_PB4); // RX
	pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA21); // TX
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;

	/* Configura opcoes USART */
	const sam_usart_opt_t usart_settings = {
		.baudrate       = 115200,
		.char_length    = US_MR_CHRL_8_BIT,
		.parity_type    = US_MR_PAR_NO,
		.stop_bits   	= US_MR_NBSTOP_1_BIT	,
		.channel_mode   = US_MR_CHMODE_NORMAL
	};

	/* Ativa Clock periferico USART0 */
	sysclk_enable_peripheral_clock(USART_COM_ID);

	/* Configura USART para operar em modo RS232 */
	usart_init_rs232(USART_COM, &usart_settings, sysclk_get_peripheral_hz());

	/* Enable the receiver and transmitter. */
	usart_enable_tx(USART_COM);
	usart_enable_rx(USART_COM);

	/* map printf to usart */
	ptr_put = (int (*)(void volatile*,char))&usart_serial_putchar;
	ptr_get = (void (*)(void volatile*,char*))&usart_serial_getchar;

	/* ativando interrupcao */
	usart_enable_interrupt(USART_COM, US_IER_RXRDY);
	NVIC_SetPriority(USART_COM_ID, 4);
	NVIC_EnableIRQ(USART_COM_ID);

}

static void configure_console(void){
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#if (defined CONF_UART_CHAR_LENGTH)
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#if (defined CONF_UART_STOP_BITS)
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
#if defined(__GNUC__)
	setbuf(stdout, NULL);
#else
	/* Already the case in IAR's Normal DLIB default configuration: printf()
	 * emits one character at a time.
	 */
#endif
}

uint32_t usart_puts(uint8_t *pstring){
	uint32_t i ;

	while(*(pstring + i))
		if(uart_is_tx_empty(USART1))
			usart_serial_putchar(USART1, *(pstring+i++));
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/

/**
 *  \brief FreeRTOS Real Time Kernel example entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
int main(void){
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the console uart */
	configure_console();
	USART1_init();

	/* Create task to make led blink */
	if (xTaskCreate(task_counter, "Led", TASK_COUNTER_STACK_SIZE, NULL,
			TASK_COUNTER_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create test led task\r\n");
	}

	/* Create task to monitor processor activity */
	if (xTaskCreate(task_uartTx, "UartTx", TASK_UARTTX_STACK_SIZE, NULL,
					TASK_UARTTX_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create UartTx task\r\n");
	}

	/* Create task to monitor processor activity */
	if (xTaskCreate(task_uartRx, "UartRx", TASK_UARTRX_STACK_SIZE, NULL,
					TASK_UARTRX_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create UartRx task\r\n");
	}

	/* Create task to monitor processor activity */
	if (xTaskCreate(task_Process, "process", TASK_PROCESS_STACK_SIZE, NULL,
	TASK_PROCESS_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create UartRx task\r\n");
	}

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){	}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
