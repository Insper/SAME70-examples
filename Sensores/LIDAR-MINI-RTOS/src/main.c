/**
 * \file
 *
 * \brief FreeRTOS configuration
 *
 * Copyright (c) 2012-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

/**
 * \mainpage FreeRTOS Real Time Kernel example
 *
 * \section Purpose
 *
 * The FreeRTOS example will help users how to use FreeRTOS in SAM boards.
 * This basic application shows how to create task and get information of
 * created task.
 *
 * \section Requirements
 *
 * This package can be used with SAM boards.
 *
 * \section Description
 *
 * The demonstration program create two task, one is make LED on the board
 * blink at a fixed rate, and another is monitor status of task.
 *
 * \section Usage
 *
 * -# Build the program and download it inside the evaluation board. Please
 *    refer to the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6224.pdf">
 *    SAM-BA User Guide</a>, the
 *    <a href="http://www.atmel.com/dyn/resources/prod_documents/doc6310.pdf">
 *    GNU-Based Software Development</a>
 *    application note or to the
 *    <a href="ftp://ftp.iar.se/WWWfiles/arm/Guides/EWARM_UserGuide.ENU.pdf">
 *    IAR EWARM User Guide</a>,
 *    depending on your chosen solution.
 * -# On the computer, open and configure a terminal application
 *    (e.g. HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# Start the application.
 * -# LED should start blinking on the board. In the terminal window, the
 *    following text should appear (values depend on the board and chip used):
 *    \code
	-- Freertos Example xxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
\endcode
 *
 */

#include "conf_board.h"
#include <asf.h>

#include "config.h"

#define TASK_LED_STACK_SIZE (2048 / sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY (tskIDLE_PRIORITY)

extern void
vApplicationStackOverflowHook(xTaskHandle *pxTask,
                              signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/**
 * \brief Called if stack overflow during execution
 */
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
                                          signed char *pcTaskName) {
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
extern void vApplicationIdleHook(void) {
  pmc_sleep(SAM_PM_SMODE_SLEEP_WFI);
}

/**
 * \brief This function is called by FreeRTOS each tick
 */
extern void vApplicationTickHook(void) {
}

extern void vApplicationMallocFailedHook(void) {
  /* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

  /* Force an assert. */
  configASSERT((volatile void *)NULL);
}

/**
 * \brief Configure the console UART.
 */
static void configure_console(void) {
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

static void configure_USART0(void) {

  /* Configura USART0 Pinos */
  sysclk_enable_peripheral_clock(ID_PIOB);
  pio_set_peripheral(PIOB, PIO_PERIPH_C, PIO_PB0);
  pio_set_peripheral(PIOB, PIO_PERIPH_C, PIO_PB1);

  /* Configura opcoes USART */
  const sam_usart_opt_t usart_settings = {
      .baudrate = 115200,
      .char_length = US_MR_CHRL_8_BIT,
      .parity_type = US_MR_PAR_NO,
      .stop_bits = US_MR_NBSTOP_1_BIT,
      .channel_mode = US_MR_CHMODE_NORMAL};

  /* Ativa Clock periferico USART0 */
  sysclk_enable_peripheral_clock(ID_USART0);

  /* Configura USART para operar em modo RS232 */
  usart_init_rs232(USART0, &usart_settings, sysclk_get_peripheral_hz());

  /* Enable the receiver and transmitter. */
  usart_enable_tx(USART0);
  usart_enable_rx(USART0);
}

uint32_t usart_putString(uint8_t *pstring) {
  uint32_t i = 0;

  while (*(pstring + i)) {
    usart_serial_putchar(USART0, *(pstring + i++));
    while (!uart_is_tx_empty(USART0)) {
    };
  }

  return (i);
}

uint32_t usart_getString(uint8_t *pstring) {
  uint32_t i = 0;

  usart_serial_getchar(USART0, (pstring + i));
  while (*(pstring + i) != '\n') {
    usart_serial_getchar(USART0, (pstring + (++i)));
  }
  *(pstring + i + 1) = 0x00;
  return (i);
}

int lidar_getMeasure(void) {

  int numCharsRead = 0;
  uint8_t lastChar = 0x00;
  uint8_t tmpChar = 0x00;
  uint8_t curChar = 0x00;

  uint16_t distance;
  uint16_t strength;
  uint16_t state;

  // Step 1: Read the serial stream until we see the beginning of the TF Mini header, or we timeout reading too many characters.
  while (1) {

    usart_serial_getchar(USART0, &curChar);

    if ((lastChar == 0x59) && (curChar == 0x59)) {
      // Break to begin frame
      break;

    } else {
      // We have not seen two 0x59's in a row -- store the current character and continue reading.
      lastChar = curChar;
      numCharsRead++;
    }

    // Error detection:  If we read more than X characters without finding a frame header, then it's likely there is an issue with
    // the Serial connection, and we should timeout and throw an error.
    if (numCharsRead > TFMINI_MAXBYTESBEFOREHEADER) {
      state = ERROR_SERIAL_NOHEADER;
      distance = -1;
      strength = -1;
      if (TFMINI_DEBUGMODE == 1) {
        printf("ERROR: no header");
      }
      return -1;
    }
  }

  // Step 2: Read one frame from the TFMini
  uint8_t frame[TFMINI_FRAME_SIZE];

  uint8_t checksum = 0x59 + 0x59;
  for (int i = 0; i < TFMINI_FRAME_SIZE; i++) {
    // Read one character
    usart_serial_getchar(USART0, &curChar);
    frame[i] = curChar;

    // Store running checksum
    if (i < TFMINI_FRAME_SIZE - 2) {
      checksum += frame[i];
    }
  }

  // Step 2A: Compare checksum
  // Last byte in the frame is an 8-bit checksum
  uint8_t checksumByte = frame[TFMINI_FRAME_SIZE - 1];
  if (checksum != checksumByte) {
    state = ERROR_SERIAL_BADCHECKSUM;
    distance = -1;
    strength = -1;
    if (TFMINI_DEBUGMODE == 1)
      printf("ERROR: bad checksum");
    return -1;
  }

  // Step 3: Interpret frame
  uint16_t dist = (frame[1] << 8) + frame[0];
  uint16_t st = (frame[3] << 8) + frame[2];
  uint8_t reserved = frame[4];
  uint8_t originalSignalQuality = frame[5];

  // Step 4: Store values
  // distance = dist;
  // strength = st;
  // state = MEASUREMENT_OK;

  // Return success
  printf("%d\n", dist);
  return dist;
}

void usart_put_char(Usart *usart, unsigned char n) {
  usart_serial_putchar(usart, n);
}

static void task_lidar(void *pvParameters) {
  // UNUSED(pvParameters);
  configure_USART0();

  usart_put_char(USART0, 66);
  usart_put_char(USART0, 87);
  usart_put_char(USART0, 2);
  usart_put_char(USART0, 0);
  usart_put_char(USART0, 0);
  usart_put_char(USART0, 0);
  usart_put_char(USART0, 1);
  usart_put_char(USART0, 6);

  for (;;) {
    lidar_getMeasure();
    vTaskDelay(10);
  }
}

int main(void) {
  /* Initialize the SAM system */
  sysclk_init();
  board_init();

  /* Initialize the console uart */
  configure_console();

  /* Output demo information. */
  printf("COMECANDO\n\r");
  printf("-- Freertos Example --\n\r");
  printf("-- %s\n\r", BOARD_NAME);
  printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

  /* Create task to monitor processor activity */
  // if (xTaskCreate(task_monitor, "Monitor", TASK_MONITOR_STACK_SIZE, NULL,
  //                 TASK_MONITOR_STACK_PRIORITY, NULL) != pdPASS) {
  //   printf("Failed to create Monitor task\r\n");
  // }

  /* Create task to make led blink */
  if (xTaskCreate(task_lidar, "lidar", TASK_LED_STACK_SIZE, NULL,
                  TASK_LED_STACK_PRIORITY, NULL) != pdPASS) {
    printf("Failed to create test lidar task\r\n");
  }
  /* Create serial to read LIDAR */
  // if (xTaskCreate(task_led, "Led", TASK_LED_STACK_SIZE, NULL,
  //                 TASK_LED_STACK_PRIORITY, NULL) != pdPASS) {
  //   printf("Failed to create test led task\r\n");
  // }

  /* Start the scheduler. */
  vTaskStartScheduler();

  /* Will only get here if there was insufficient memory to create the idle task. */

  while (1) {
  }
  return 0;
}
