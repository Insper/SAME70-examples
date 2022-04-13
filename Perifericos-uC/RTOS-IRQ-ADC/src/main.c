#include "conf_board.h"
#include <asf.h>

/************************************************************************/
/* BOARD CONFIG                                                         */
/************************************************************************/

#define USART_COM_ID ID_USART1
#define USART_COM USART1

#define AFEC_POT AFEC1
#define AFEC_POT_ID ID_AFEC1
#define AFEC_POT_CHANNEL 6 // Canal do pino PC31

/************************************************************************/
/* RTOS                                                                */
/************************************************************************/

#define TASK_ADC_STACK_SIZE (1024 / sizeof(portSTACK_TYPE))
#define TASK_ADC_STACK_PRIORITY (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                          signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/************************************************************************/
/* recursos RTOS                                                        */
/************************************************************************/

/** Queue for msg log send data */
QueueHandle_t xQueueTouch;

typedef struct {
  uint value;
} adcData;

/************************************************************************/
/* prototypes local                                                     */
/************************************************************************/

static void USART1_init(void);
TC_init(Tc *TC, int ID_TC, int TC_CHANNEL, int freq);
config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel,
                afec_callback_t callback);
configure_console(void);

/************************************************************************/
/* RTOS application funcs                                               */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                          signed char *pcTaskName) {
  printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
  /* If the parameters have been corrupted then inspect pxCurrentTCB to
   * identify which task has overflowed its stack.
   */
  for (;;) {
  }
}

extern void vApplicationIdleHook(void) { pmc_sleep(SAM_PM_SMODE_SLEEP_WFI); }

extern void vApplicationTickHook(void) {}

extern void vApplicationMallocFailedHook(void) {
  /* Called if a call to pvPortMalloc() fails because there is insufficient
  free memory available in the FreeRTOS heap.  pvPortMalloc() is called
  internally by FreeRTOS API functions that create tasks, queues, software
  timers, and semaphores.  The size of the FreeRTOS heap is set by the
  configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

  /* Force an assert. */
  configASSERT((volatile void *)NULL);
}

/************************************************************************/
/* handlers / callbacks                                                 */
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
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(xQueueADC, &adc, &xHigherPriorityTaskWoken);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_adc(void *pvParameters) {

  // configura ADC e TC para controlar a leitura
  config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_Callback);
  TC_init(TC0, ID_TC1, 1, 60);

  // variável para recever dados da fila
  adcData adc;

  while (1) {
    if (xQueueReceive(xQueueADC, &(adc), 1000)) {
      printf("ADC: %d", adc);
    } else {
      printf("Nao chegou um novo dado em 1 segundo")
    }
  }
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

/**
 * \brief Configure the console UART.
 */
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

  tc_start(TC, TC_CHANNEL);
}

static void USART1_init(void) {
  /* Configura USART1 Pinos */
  sysclk_enable_peripheral_clock(ID_PIOB);
  sysclk_enable_peripheral_clock(ID_PIOA);
  pio_set_peripheral(PIOB, PIO_PERIPH_D, PIO_PB4);  // RX
  pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA21); // TX
  MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;

  /* Configura opcoes USART */
  const sam_usart_opt_t usart_settings = {.baudrate = 115200,
                                          .char_length = US_MR_CHRL_8_BIT,
                                          .parity_type = US_MR_PAR_NO,
                                          .stop_bits = US_MR_NBSTOP_1_BIT,
                                          .channel_mode = US_MR_CHMODE_NORMAL};

  /* Ativa Clock periferico USART0 */
  sysclk_enable_peripheral_clock(USART_COM_ID);

  /* Configura USART para operar em modo RS232 */
  usart_init_rs232(USART_COM, &usart_settings, sysclk_get_peripheral_hz());

  /* Enable the receiver and transmitter. */
  usart_enable_tx(USART_COM);
  usart_enable_rx(USART_COM);

  /* map printf to usart */
  ptr_put = (int (*)(void volatile *, char)) & usart_serial_putchar;
  ptr_get = (void (*)(void volatile *, char *)) & usart_serial_getchar;

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
int main(void) {
  /* Initialize the SAM system */
  sysclk_init();
  board_init();
  USART1_init();
  configure_console();

  xQueueADC = xQueueCreate(100, sizeof(adcData));
  if (xSemaphore == NULL)
    printf("falha em criar o semaforo \n");

  /* Create task to make led blink */
  if (xTaskCreate(task_adc, "ADC", TASK_ADC_STACK_SIZE, NULL,
                  TASK_ADC_STACK_PRIORITY, NULL) != pdPASS) {
    printf("Failed to create test ADC task\r\n");
  }

  /* Start the scheduler. */
  vTaskStartScheduler();

  /* RTOS não deve chegar aqui !! */
  while (1) {
  }

  /* Will only get here if there was insufficient memory to create the idle
   * task. */
  return 0;
}
