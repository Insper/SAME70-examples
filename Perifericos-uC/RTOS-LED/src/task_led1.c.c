/*
* Example RTOS Atmel Studio
*/

#include <asf.h>
#include "conf_board.h"

/* Pinos do LED1 */
#define LED1_PIO			PIOA
#define LED1_PIO_ID			ID_PIOA
#define LED1_PIO_IDX		0
#define LED1_PIO_IDX_MASK	(1 << LED1_PIO_IDX)

#define TASK_LED1_STACK_SIZE  (1024/sizeof(portSTACK_TYPE))
#define TASK_LED1_STACK_PRIORITY (tskIDLE_PRIORITY)
#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)
#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,	signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

void pisca_led(int n, int t);

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



/**
* \brief This task, when activated, send every ten seconds on debug UART
* the whole report of free heap and total tasks status
*/
static void task_monitor(void *pvParameters)
{
  static portCHAR szList[256];
  UNUSED(pvParameters);

  for (;;) {
    printf("--- task ## %u", (unsigned int)uxTaskGetNumberOfTasks());
    vTaskList((signed portCHAR *)szList);
    printf(szList);
    vTaskDelay(1000);
  }
}

/**
* \brief This task, when activated, make LED blink at a fixed rate
*/
static void task_led(void *pvParameters){
  UNUSED(pvParameters);
  const TickType_t xDelay = 2000 / portTICK_PERIOD_MS;

  for (;;) {
    LED_Toggle(LED0);
    vTaskDelay(xDelay);
  }
}

static void task_led1(void *pvParameters){
  pmc_enable_periph_clk(LED1_PIO_ID);
  pio_configure(LED1_PIO, PIO_OUTPUT_0, LED1_PIO_IDX_MASK, PIO_DEFAULT);

  const TickType_t xDelay = 3000 / portTICK_PERIOD_MS;
  const TickType_t xDelayLed = 200 / portTICK_PERIOD_MS;

  for (;;) {
    for (uint i=0; i<5; i++){
      pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);
      vTaskDelay(xDelayLed);
      pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
      vTaskDelay(xDelayLed);
    }
      vTaskDelay(xDelay);
  }
}

/**
* \brief Configure the console UART.
*/
static void configure_console(void)
{
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
  configure_console();

  /* Output demo information. */
  printf("-- Freertos Example --\n\r");
  printf("-- %s\n\r", BOARD_NAME);
  printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);


  /* Create task to monitor processor activity */
  if (xTaskCreate(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL,
  TASK_MONITOR_STACK_PRIORITY, NULL) != pdPASS) {
    printf("Failed to create Monitor task\r\n");
  }

  /* Create task to make led blink */
  if (xTaskCreate(task_led, "Led", TASK_LED_STACK_SIZE, NULL,
  TASK_LED_STACK_PRIORITY, NULL) != pdPASS) {
    printf("Failed to create test led task\r\n");
  }

 /* Create task to make led blink */
 if (xTaskCreate(task_led1, "Led1", TASK_LED1_STACK_SIZE, NULL,
 TASK_LED1_STACK_PRIORITY, NULL) != pdPASS) {
   printf("Failed to create test led task\r\n");
 }
 
  /* Start the scheduler. */
  vTaskStartScheduler();

  /* Will only get here if there was insufficient memory to create the idle task. */
  return 0;
}
