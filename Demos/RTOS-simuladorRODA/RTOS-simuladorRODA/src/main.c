#include <asf.h>
#include "conf_board.h"
#include "arm_math.h"

// LED
#define LED_RAMP_PIO      PIOC
#define LED_RAMP_PIO_ID   ID_PIOC
#define LED_RAMP_IDX      8
#define LED_RAMP_IDX_MASK (1 << LED_RAMP_IDX)

// Botão
#define BUT_RAMP_PIO      PIOA
#define BUT_RAMP_PIO_ID   ID_PIOA
#define BUT_RAMP_IDX  11
#define BUT_RAMP_IDX_MASK (1 << BUT_RAMP_IDX)


/************************************************************************/
/* RTOS                                                                */
/************************************************************************/

/** RTOS  */


#define TASK_SIMULATOR_STACK_SIZE (4096 / sizeof(portSTACK_TYPE))
#define TASK_SIMULATOR_STACK_PRIORITY (tskIDLE_PRIORITY)

#define TASK_LED_STACK_SIZE (4096 / sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY (tskIDLE_PRIORITY)

#define RAIO 0.508/2
#define VEL_MAX_KMH  5.0f
#define VEL_MIN_KMH  0.5f


extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/************************************************************************/
/* recursos RTOS                                                        */
/************************************************************************/

/** Semaforo a ser usado pela task led */
SemaphoreHandle_t xSemaphoreBut_RAMP;
SemaphoreHandle_t xSemaphoreLED;

/** prototypes */
void but_callback(void);
float kmh_to_hz(float vel, float raio);
void io_init(void);
void pin_toggle(Pio *pio, uint32_t mask);
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

void but_RAMP_callback(void)
{
	 BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	 xSemaphoreGiveFromISR(xSemaphoreBut_RAMP, &xHigherPriorityTaskWoken);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_simulador(void *pvParameters) {


	pmc_enable_periph_clk(ID_PIOC);
	pio_set_output(PIOC, PIO_PC31, 1, 0, 0);

	float vel = VEL_MAX_KMH;
	float f;
	int ramp_up = 1;
	int ramp_status = 0;

	while(1){
		
		if (xSemaphoreTake(xSemaphoreBut_RAMP, 1)) {
			ramp_status = !ramp_status;
			 xSemaphoreGive(xSemaphoreLED);
		}
		pio_clear(PIOC, PIO_PC31);
		delay_ms(1);
		pio_set(PIOC, PIO_PC31);
		
		
		if(ramp_status == 1){
			if (ramp_up) {
				printf("[SIMU] ACELERANDO: %d \n", (int) (10*vel));
				vel += 0.5;
				} else {
				printf("[SIMU] DESACELERANDO: %d \n",  (int) (10*vel));
				vel -= 0.5;
			}

			if (vel >= VEL_MAX_KMH)
			ramp_up = 0;
			else if (vel <= VEL_MIN_KMH)
			ramp_up = 1;
		}
		
		else{
			vel = 5;
			printf("[SIMU] CONSTANTE: %d \n", (int) (10*vel));
		}
		
		f = kmh_to_hz(vel, RAIO);
		int t = 965*(1.0/f); //UTILIZADO 965 como multiplicador ao invés de 1000
							 //para compensar o atraso gerado pelo Escalonador do freeRTOS
		delay_ms(t);
	}
	
}

static void task_led(void *pvParameters) {
	
	
	
	 
	int ramp_status = 0;
	
	while(1){
		
		if (xSemaphoreTake(xSemaphoreLED, 1)) {
			ramp_status = !ramp_status;
		}
		
		if(ramp_status == 1) {
			
			pin_toggle(LED_RAMP_PIO, LED_RAMP_IDX_MASK); 
		}
		
		else {
			pio_set(LED_RAMP_PIO, LED_RAMP_IDX_MASK); 
		}
		
	}
}







/************************************************************************/
/* funcoes                                                              */
/************************************************************************/
void pin_toggle(Pio *pio, uint32_t mask) {
	if(pio_get_output_data_status(pio, mask))
	pio_clear(pio, mask);
	else
	pio_set(pio,mask);
}


void io_init(void)
{

	// Configura led
	pmc_enable_periph_clk(LED_RAMP_PIO_ID);
	pio_configure(LED_RAMP_PIO, PIO_OUTPUT_0, LED_RAMP_IDX_MASK, PIO_DEFAULT);

	// Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUT_RAMP_PIO_ID);

	// Configura PIO para lidar com o pino do botão como entrada
	// com pull-up
	pio_configure(BUT_RAMP_PIO, PIO_INPUT, BUT_RAMP_IDX_MASK, PIO_PULLUP);
	pio_set_debounce_filter(BUT_RAMP_PIO, BUT_RAMP_IDX_MASK, 1000);

	// Configura interrupção no pino referente ao botao e associa
	// função de callback caso uma interrupção for gerada
	// a função de callback é a: but_callback()
	pio_handler_set(BUT_RAMP_PIO,
	BUT_RAMP_PIO_ID,
	BUT_RAMP_IDX_MASK,
	PIO_IT_EDGE,
	but_RAMP_callback);

	// Ativa interrupção e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(BUT_RAMP_PIO, BUT_RAMP_IDX_MASK);
	pio_get_interrupt_status(BUT_RAMP_PIO);
	
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(BUT_RAMP_PIO_ID);
	NVIC_SetPriority(BUT_RAMP_PIO_ID, 4); // Prioridade 4
}

	/**
	* raio 20" => 50,8 cm (diametro) => 0.508/2 = 0.254m (raio)
	* w = 2 pi f (m/s)
	* v [km/h] = (w*r) / 3.6 = (2 pi f r) / 3.6
	* f = v / (2 pi r 3.6)
	* Exemplo : 5 km / h = 1.38 m/s
	*           f = 0.87Hz
	*           t = 1/f => 1/0.87 = 1,149s
	*/
	float kmh_to_hz(float vel, float raio) {
		float f = vel / (2*PI*raio*3.6);
		return(f);
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
	io_init();
	
	/* Attempt to create a semaphore. */
	xSemaphoreBut_RAMP = xSemaphoreCreateBinary();
	if (xSemaphoreBut_RAMP == NULL)	 printf("falha em criar o semaforo \n");
	/* Attempt to create a semaphore. */
	xSemaphoreLED = xSemaphoreCreateBinary();
	if (xSemaphoreLED == NULL)	 printf("falha em criar o semaforo \n");
	
	if (xTaskCreate(task_simulador, "SIMUL", TASK_SIMULATOR_STACK_SIZE, NULL, TASK_SIMULATOR_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create lcd task\r\n");
	}

	if (xTaskCreate(task_led, "LED", TASK_LED_STACK_SIZE, NULL, TASK_LED_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create lcd task\r\n");
	}

	/* Start the scheduler. */
	vTaskStartScheduler();

	while(1){	}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
