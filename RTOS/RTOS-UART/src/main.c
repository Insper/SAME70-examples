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
#define ECHO_PIO      PIOA
#define ECHO_PIO_ID   ID_PIOA
#define ECHO_IDX  11
#define ECHO_IDX_MASK (1 << BUT_IDX)

// Timer
#define TC_CHANNEL 1
#define TC_ID      ID_TC1
#define TC         TC0
#define TC_FREQ    42


#define USART_COM_ID ID_USART1
#define USART_COM    USART1

/** RTOS  */
#define TASK_TRIGGER_STACK_SIZE            (1024/sizeof(portSTACK_TYPE))
#define TASK_TRIGGER_STACK_PRIORITY        (tskIDLE_PRIORITY)
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
char ucMessageTx[128];

/** Queue for IRQ -> taskRx */
QueueHandle_t xQueueRx;
char ucMessageRx;

/** Queue for taskRx -> taskProcess */
QueueHandle_t xQueueMSG;
char ucMessageMSG[128];

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

  if(pio_get(ECHO_PIO, ECHO_IDX_MASK)){
    tc_start(TC, TC_CHANNEL);
    // start timer
  }
  else{
    BaseType_t xHigherPriorityTaskWoken = pdTRUE;
    xSemaphoreGiveFromISR(xSemaphore, &xHigherPriorityTaskWoken);
  }
}

void USART1_Handler(void){
	uint32_t ret = usart_get_status(USART_COM);

	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	char c;

	// Verifica por qual motivo entrou na interrupçcao
	//  - Dadodisponível para leitura
	if(ret & US_IER_RXRDY){
		usart_serial_getchar(USART_COM, &c);
		xQueueSendFromISR(xQueueTx, (void *) &c, &xHigherPriorityTaskWoken);

		//usart_serial_putchar(USART1,c);

	// -  Transmissoa finalizada
	} else if(ret & US_IER_TXRDY){

	}
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_led(void *pvParameters){
	/* Attempt to create a semaphore. */
	xSemaphore = xSemaphoreCreateBinary();

	if (xSemaphore == NULL)
		printf("falha em criar o semaforo \n");

	uint8_t cnt = 0 ;

	for (;;) {
		if( xSemaphoreTake(xSemaphore, 500 / portTICK_PERIOD_MS) == pdTRUE ){
			//TRIGGER_Toggle(TRIGGER0);
			sprintf(ucMessageTx, "cnt = %d \n", cnt++);
			TRIGGER_Toggle(TRIGGER0);
			xQueueSend(xQueueTx, (void *) &ucMessageTx, 10);
		}
	}
}

static void task_uartTx(void *pvParameters){
	char rxMSG[128];

	xQueueTx = xQueueCreate(32, sizeof(ucMessageTx) );
	if(xQueueTx == NULL){
		printf("Fail to create xQueueTx \n");
		while(1){};
	}

	while(1){
		if( xQueueReceive( xQueueTx, &rxMSG, ( TickType_t ) 1 )){
			usart_puts(rxMSG);
		}
		else{
			vTaskDelay(3000);
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

	/* Configura o PMC */
	/* O TimerCounter é meio confuso
     o uC possui 3 TCs, cada TC possui 3 canais
     TC0 : ID_TC0, ID_TC1, ID_TC2
     TC1 : ID_TC3, ID_TC4, ID_TC5
     TC2 : ID_TC6, ID_TC7, ID_TC8
	*/
	pmc_enable_periph_clk(ID_TC);

	/** Configura o TC para operar em  4Mhz e interrupçcão no RC compare */
	//tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	//tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	/* Configura e ativa interrupçcão no TC canal 0 */
	/* Interrupção no C */
	//NVIC_EnableIRQ((IRQn_Type) ID_TC);
	//tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

	/* Inicializa o canal 0 do TC */
}

void io_init(void){

  // Configura led
	pmc_enable_periph_clk(TRIGGER_PIO_ID);
	pio_configure(TRIGGER_PIO, PIO_OUTPUT_0, TRIGGER_IDX_MASK, PIO_DEFAULT);

  // Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(ECHO_PIO_ID);

  // Configura PIO para lidar com o pino do botão como entrada
  // com pull-up
	pio_configure(ECHO_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);

  // Configura interrupção no pino referente ao botao e associa
  // função de callback caso uma interrupção for gerada
  // a função de callback é a: but_callback()
  pio_handler_set(ECHO_PIO,
                  ECHO_PIO_ID,
                  ECHO_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  signalCallback);

  // Ativa interrupção
  pio_enable_interrupt(ECHO_PIO, BUT_IDX_MASK);

  // Configura NVIC para receber interrupcoes do PIO do botao
  // com prioridade 4 (quanto mais próximo de 0 maior)
  NVIC_EnableIRQ(ECHO_PIO_ID);
  NVIC_SetPriority(ECHO_PIO_ID, 4); // Prioridade 4
}

/************************************************************************/
/* inits                                                                */
/************************************************************************/

static void io_init(void){
	/* configura prioridae */
	NVIC_EnableIRQ(ECHO_PIO_ID);
	NVIC_SetPriority(ECHO_PIO_ID, 4);

	/* conf botão como entrada */
	pio_configure(ECHO_PIO, PIO_INPUT, BUT_PIO_PIN_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_set_debounce_filter(ECHO_PIO, BUT_PIO_PIN_MASK, 60);
	pio_enable_interrupt(ECHO_PIO, BUT_PIO_PIN_MASK);
	pio_handler_set(ECHO_PIO, BUT_PIO_ID, BUT_PIO_PIN_MASK, PIO_IT_FALL_EDGE , but_callback);

	//printf("Prioridade %d \n", NVIC_GetPriority(ECHO_PIO_ID));
}

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

  // inicializa TC
	TC_init(TC, TC_ID, TC_CHANNEL, TC_FREQ);

	/* iniciliza pinos */
	io_init();


  while(1){
    /* - Gera trigger no pino
     *                _____
     * Trigger  _____|     |________
     *                 10us
    */
    pio_set(TRIGGER_PIO, TRIGGER_IDX_MASK);
    delay_us(10);
    pio_clear(TRIGGER_PIO, TRIGGER_IDX_MASK);
    tc_start(TC, TC_CHANNEL);


    /* - Aguarda sinal do Echo
     *                ____________
     * Echo     _____|            |________
     *               ^            ^
     *               |            |
     *           Inicializa TC  Faz leitura TC
     */
    while(!pio_get(ECHO_PIO, ECHO_IDX_MASK)){}
    uint32_t tcCv = tc_read_cv(Tc *p_tc, uint32_t ul_channel);
    tc_stop(TC, TC_CHANNEL);

    /* D = t(s) * Vel Sound (m/s) / 2 */
    float ts = (foat) tcCV / 32000.0;
    float dm = ts * 340/2;

    printf("%d", (int) (dm*100));

    delay_s(1);
  }

  

	// esse delay é necessário mas não entendo o porque !
	// sem ele o freertos considera que a interrupcao
	// do botao tem prioridade maior que a do systick
	// entrando em modo configASSERT
	// estudar :
	//  - https://dzone.com/articles/arm-cortex-m-interrupts-and-freertos-part-1
	//  - https://www.freertos.org/RTOS-Cortex-M3-M4.html
	delay_ms(100);


	/* Create task to make led blink */
	if (xTaskCreate(task_led, "Led", TASK_TRIGGER_STACK_SIZE, NULL,
			TASK_TRIGGER_STACK_PRIORITY, NULL) != pdPASS) {
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
	if (xTaskCreate(task_Process, "UartRx", TASK_PROCESS_STACK_SIZE, NULL,
	TASK_PROCESS_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create UartRx task\r\n");
	}


	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){


	}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
