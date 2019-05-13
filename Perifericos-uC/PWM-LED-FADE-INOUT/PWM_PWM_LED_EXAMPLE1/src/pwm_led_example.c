/**
 * \file
 *
 * \brief PWM LED example for SAM.
 *
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
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

/**
 * \mainpage PWM LED Example
 *
 * \par Purpose
 *
 * This example demonstrates a simple configuration of 2 PWM channels to
 * generate variable duty cycle signals.
 * The 2 LEDs on the evaluation kit will glow repeatedly.
 *
 * \section Requirements
 *
 * This example can be used on SAM boards. The 2 required leds need to
 * be connected to PWM output pins, else consider probing the PWM output pins
 * with an oscilloscope.
 *
 * \par Usage
 *
 * -# Initialize system clock and pins setting on board
 * -# Initialize PWM clock
 * -# Configure PIN_PWM_LED0_CHANNEL
 * -# Configure PIN_PWM_LED1_CHANNEL
 * -# Enable interrupt of counter event and PIN_PWM_LED0_CHANNEL
 * & PIN_PWM_LED1_CHANNEL
 * -# Change duty cycle in ISR
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

#define PIO_PWM_0 PIOA
#define ID_PIO_PWM_0 ID_PIOA
#define MASK_PIN_PWM_0 (1 << 0) 

/** PWM frequency in Hz */
#define PWM_FREQUENCY      1000
/** Period value of PWM output waveform */
#define PERIOD_VALUE       100
/** Initial duty cycle value */
#define INIT_DUTY_VALUE    0

/** PWM channel instance for LEDs */
pwm_channel_t g_pwm_channel_led;

/**
 *  \brief Configure the Console UART.
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
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

void PWM0_init(uint channel, uint duty){
  /* Enable PWM peripheral clock */
  pmc_enable_periph_clk(ID_PWM0);

  /* Disable PWM channels for LEDs */
  pwm_channel_disable(PWM0, PIN_PWM_LED0_CHANNEL);

  /* Set PWM clock A as PWM_FREQUENCY*PERIOD_VALUE (clock B is not used) */
  pwm_clock_t clock_setting = {
    .ul_clka = PWM_FREQUENCY * PERIOD_VALUE,
    .ul_clkb = 0,
    .ul_mck = sysclk_get_peripheral_hz()
  };
  
  pwm_init(PWM0, &clock_setting);

  /* Initialize PWM channel for LED0 */
  /* Period is left-aligned */
  g_pwm_channel_led.alignment = PWM_ALIGN_CENTER;
  /* Output waveform starts at a low level */
  g_pwm_channel_led.polarity = PWM_HIGH;
  /* Use PWM clock A as source clock */
  g_pwm_channel_led.ul_prescaler = PWM_CMR_CPRE_CLKA;
  /* Period value of output waveform */
  g_pwm_channel_led.ul_period = PERIOD_VALUE;
  /* Duty cycle value of output waveform */
  g_pwm_channel_led.ul_duty = duty;
  g_pwm_channel_led.channel = channel;
  pwm_channel_init(PWM0, &g_pwm_channel_led);
 
  /* Enable PWM channels for LEDs */
  pwm_channel_enable(PWM0, channel);
}

/**
 * \brief Application entry point for PWM with LED example.
 * Output PWM waves on LEDs to make them fade in and out.
 */
int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Configure the console uart for debug information */
	configure_console();
  printf("Exemplo de PWM fade in / fade out \n");
  
  /* Configura pino para ser controlado pelo PWM */
  pmc_enable_periph_clk(ID_PIO_PWM_0);
  pio_set_peripheral(PIO_PWM_0, PIO_PERIPH_A, MASK_PIN_PWM_0 );
  
  /* inicializa PWM com dutycicle 0*/
  uint duty = 0;
  PWM0_init(0, duty);

	/* Infinite loop */
	while (1) {
    /* fade in */
    for(duty = 0; duty <= 100; duty++){
      pwm_channel_update_duty(PWM0, &g_pwm_channel_led, 100-duty);
      delay_ms(10);
    }   
    /* fade out*/
    for(duty = 0; duty <= 100; duty++){
      pwm_channel_update_duty(PWM0, &g_pwm_channel_led, duty);
      delay_ms(10);
    }
 	}
}
