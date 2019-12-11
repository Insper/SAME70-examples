#include <asf.h>
#include "conf_board.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

// TRIGGER
#define TRIGGER_PIO      PIOC
#define TRIGGER_PIO_ID   ID_PIOC
#define TRIGGER_IDX      8
#define TRIGGER_IDX_MASK (1 << TRIGGER_IDX)

// Botão
#define ECHO_PIO      PIOD
#define ECHO_PIO_ID   ID_PIOD
#define ECHO_IDX      20
#define ECHO_IDX_MASK (1 << ECHO_IDX)

// Timer
#define TIMER_CHANNEL 1
#define TIMER_ID      ID_TC1
#define TIMER         TC0
#define TIMER_FREQ    42

#define USART_COM_ID ID_USART1
#define USART_COM    USART1

/** RTOS  */
#define TASK_SENSOR_STACK_SIZE            (1024/sizeof(portSTACK_TYPE))
#define TASK_SENSOR_STACK_PRIORITY        (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

QueueHandle_t xQueue1;

/** prototypes */
void but_callback(void);
static void ECHO_init(void);
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

void signalCallback(void){
  if(!pio_get(ECHO_PIO, PIO_INPUT, ECHO_IDX_MASK)){
    tc_start(TIMER, TIMER_CHANNEL);
  }else{
   tc_stop(TIMER, TIMER_CHANNEL);
   uint tcCv = tc_read_cv(TIMER, TIMER_CHANNEL);
   xQueueSendFromISR( xQueue1, &tcCv, NULL );
  }
}



/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

/**
 * \brief Configure the console UART.
 */

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

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	uint32_t channel = 1;

	pmc_enable_periph_clk(ID_TC);

	tc_init(TC, TC_CHANNEL, TC_CMR_TCCLKS_TIMER_CLOCK5 | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, 65535);
}

void io_init(void){

  // Configura led
	pmc_enable_periph_clk(TRIGGER_PIO_ID);
	pio_configure(TRIGGER_PIO, PIO_OUTPUT_0, TRIGGER_IDX_MASK, PIO_DEFAULT);

  // Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(ECHO_PIO_ID);

  // Configura PIO para lidar com o pino do botão como entrada
  // com pull-up
	pio_configure(ECHO_PIO, PIO_INPUT, ECHO_IDX_MASK, PIO_PULLUP);

  // Configura interrupção no pino referente ao botao e associa
  // função de callback caso uma interrupção for gerada
  // a função de callback é a: but_callback()
  pio_handler_set(ECHO_PIO,
                  ECHO_PIO_ID,
                  ECHO_IDX_MASK,
                  PIO_IT_EDGE,
                  signalCallback);

  // Ativa interrupção
  pio_enable_interrupt(ECHO_PIO, ECHO_IDX_MASK);

  // Configura NVIC para receber interrupcoes do PIO do botao
  // com prioridade 4 (quanto mais próximo de 0 maior)
  NVIC_EnableIRQ(ECHO_PIO_ID);
  NVIC_SetPriority(ECHO_PIO_ID, 4); // Prioridade 4
}

/************************************************************************/
/* inits                                                                */
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

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

void task_sensor(void){

  /* Create a queue capable of containing 10 unsigned long values. */
  xQueue1 = xQueueCreate( 10, sizeof( uint ) );

  TC_init(TIMER, TIMER_ID, TIMER_CHANNEL, TIMER_FREQ);
  io_init();

  uint tcCv;
  while(1){

    pio_set(TRIGGER_PIO, TRIGGER_IDX_MASK);
    delay_us(10);   // precisa ser esse delay, não pode trocar pelo vTaskDelay! 
    pio_clear(TRIGGER_PIO, TRIGGER_IDX_MASK);
    printf("Trigger \n");
   if( xQueueReceive( xQueue1, &(tcCv), ( TickType_t )  500 / portTICK_PERIOD_MS ) )
   {
      /* D = t(s) * Vel Sound (m/s) / 2 */
      float ts = (float) tcCv / 32000.0;
      float dm = ts * 340/2;
      printf("%d\n", (int) (dm*100));
   }
   else{
     printf("Erro\n");
   }

   vTaskDelay( 500 / portTICK_PERIOD_MS);
  }
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

	/* Create task to make led blink */
	if (xTaskCreate(task_sensor, "Sensor", TASK_SENSOR_STACK_SIZE, NULL,
			TASK_SENSOR_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create test led task\r\n");
	}

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){	}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
