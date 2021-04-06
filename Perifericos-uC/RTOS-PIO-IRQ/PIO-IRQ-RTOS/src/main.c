#include <asf.h>
#include "conf_board.h"


/** Borad */

/* Botao da placa */
#define BUT_PIO     PIOA
#define BUT_PIO_ID  ID_PIOA
#define BUT_PIO_PIN 11
#define BUT_PIO_PIN_MASK (1 << BUT_PIO_PIN)

#define USART_COM_ID ID_USART1
#define USART_COM    USART1

/** RTOS  */
#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)
#define TASK_BUT_STACK_SIZE             (2048/sizeof(portSTACK_TYPE))
#define TASK_BUT_STACK_PRIORITY         (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/** Semaforo a ser usado pela task led */
SemaphoreHandle_t xSemaphoreBut;

/** Queue for msg log send data */
QueueHandle_t xQueueLedFreq;
uint32_t ucMsgLedFreq;

/** prototypes */
void but_callback(void);
static void BUT_init(void);
static void USART1_init(void);

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

void but_callback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xSemaphoreGiveFromISR(xSemaphoreBut, &xHigherPriorityTaskWoken);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_led(void *pvParameters)
{
	/* cria queue com 32 "espacos" */
	/* cada espaço possui o tamanho da variavel uCMsgLedFreq */
	xQueueLedFreq = xQueueCreate(32, sizeof(ucMsgLedFreq) );
	if (xQueueLedFreq == NULL)
		printf("falha em criar a queue \n");

	uint32_t msg = 0 ;
	uint32_t delayMs = 900 ;

	/* tarefas de um RTOS não devem retornar */
	for (;;)
  {
		/* verifica se chegou algum dado na queue, caso contrario */
		/* timeout = 0 */
		if( xQueueReceive(xQueueLedFreq, &msg, ( TickType_t ) 0 ))
    {
			/* chegou novo valor, atualiza delay ! */
			/* aqui eu poderia verificar se msg faz sentido (se esta no range certo) */
			/* converte ms -> ticks */
			delayMs = msg / portTICK_PERIOD_MS;
			printf("delay = %d \n", delayMs );
		}
		/* pisca LED */
		LED_Toggle(LED0);

		/* suspende por delayMs */
		vTaskDelay(delayMs);
	}
}

static void task_but(void *pvParameters)
{
	/* Attempt to create a semaphore. */
	xSemaphoreBut = xSemaphoreCreateBinary();
	if (xSemaphoreBut == NULL)
		printf("falha em criar o semaforo \n");

	uint32_t delayTicks = 900 ;

	for (;;) {
		/* aguarda por tempo inderteminado até a liberacao do semaforo */
		if( xSemaphoreTake(xSemaphoreBut, 0))
    {
			/* atualiza frequencia */
			delayTicks -= 100;

			/* envia nova frequencia para a task_led */
			xQueueSend(xQueueLedFreq, (void *) &delayTicks, 10);

			/* garante range da freq. */
			if(delayTicks == 100){
				delayTicks = 900;
			}
		}
	}
}


/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

/**
 * \brief Configure the console UART.
 */
static void configure_console(void)
{
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
/* inits                                                                */
/************************************************************************/

static void BUT_init(void)
{
	/* configura prioridae */
	NVIC_EnableIRQ(BUT_PIO_ID);
	NVIC_SetPriority(BUT_PIO_ID, 4);

	/* conf botão como entrada */
	pio_configure(BUT_PIO, PIO_INPUT, BUT_PIO_PIN_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(BUT_PIO, BUT_PIO_PIN_MASK, 60);
	pio_enable_interrupt(BUT_PIO, BUT_PIO_PIN_MASK);
	pio_handler_set(BUT_PIO, BUT_PIO_ID, BUT_PIO_PIN_MASK, PIO_IT_FALL_EDGE , but_callback);
}

static void USART1_init(void)
{
	/* Configura USART1 Pinos */
	sysclk_enable_peripheral_clock(ID_PIOB);
	sysclk_enable_peripheral_clock(ID_PIOA);
	pio_set_peripheral(PIOB, PIO_PERIPH_D, PIO_PB4); // RX
	pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA21); // TX
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;

	/* Configura opcoes USART */
	const sam_usart_opt_t usart_settings =
  {
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

/************************************************************************/
/* main                                                                 */
/************************************************************************/

/**
 *  \brief FreeRTOS Real Time Kernel example entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Initialize the console uart */
	USART1_init();
	configure_console();

	/* iniciliza botao */
	BUT_init();

	// sem ele o freertos considera que a interrupcao
	// do botao tem prioridade maior que a do systick
	// entrando em modo configASSERT
	// estudar :
	//  - https://dzone.com/articles/arm-cortex-m-interrupts-and-freertos-part-1
	//  - https://www.freertos.org/RTOS-Cortex-M3-M4.html
	delay_ms(100);

	/* Create task to make led blink */
	if (xTaskCreate(task_led, "Led", TASK_LED_STACK_SIZE, NULL,
			TASK_LED_STACK_PRIORITY, NULL) != pdPASS)
  {
		printf("Failed to create test led task\r\n");
	}

	/* Create task to monitor processor activity */
	if (xTaskCreate(task_but, "UartTx", TASK_BUT_STACK_SIZE, NULL,
					TASK_BUT_STACK_PRIORITY, NULL) != pdPASS)
  {
		printf("Failed to create UartTx task\r\n");
	}

	/* Start the scheduler. */
	vTaskStartScheduler();

  /* RTOS não deve chegar aqui !! */
	while(1){}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
