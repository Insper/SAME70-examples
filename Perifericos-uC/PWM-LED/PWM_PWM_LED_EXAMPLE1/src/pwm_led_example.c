#include "asf.h"
#include "stdio_serial.h"
#include "conf_board.h"
#include "conf_clock.h"

#define PIO_PWM_0 PIOA
#define ID_PIO_PWM_0 ID_PIOA
#define MASK_PIN_PWM_0 (1 << 0) 

void PWM_init(Pwm *p_pwm, uint id_pwm, pwm_channel_t *p_channel, uint channel, uint duty) {
	/* Enable PWM peripheral clock */
	pmc_enable_periph_clk(id_pwm);

	/* Disable PWM channels for LEDs */
	pwm_channel_disable(p_pwm, channel);

	/* Set PWM clock A as PWM_FREQUENCY*PERIOD_VALUE (clock B is not used) */
	pwm_clock_t clock_setting = {
		.ul_clka = 1000 * 256,
		.ul_clkb = 0,
		.ul_mck = sysclk_get_peripheral_hz()
	};

	pwm_init(p_pwm, &clock_setting);

	/* Initialize PWM channel for LED0 */
	/* Period is left-aligned */
	p_channel->alignment = PWM_ALIGN_CENTER;
	/* Output waveform starts at a low level */
	p_channel->polarity = PWM_HIGH;
	/* Use PWM clock A as source clock */
	p_channel->ul_prescaler = PWM_CMR_CPRE_CLKA;
	/* Period value of output waveform */
	p_channel->ul_period = 256;
	/* Duty cycle value of output waveform */
	p_channel->ul_duty = duty;
	p_channel->channel = channel;
	pwm_channel_init(p_pwm, p_channel);

	/* Enable PWM channels for LEDs */
	pwm_channel_enable(p_pwm, channel);
}


int main(void) {
  /* Initialize the SAM system */
  sysclk_init();
  board_init();


  /* Configura pino para ser controlado pelo PWM */
  /* MUITO IMPORTANTE AJUSTAR ESSE CÓDIGO DE ACORDO COM O CANAL E PINO USADO */
  pmc_enable_periph_clk(ID_PIO_PWM_0);
  pio_set_peripheral(PIO_PWM_0, PIO_PERIPH_A, MASK_PIN_PWM_0 );

  /* inicializa PWM com duty cycle 23*/
  /* MUITO IMPORTANTE CRIAR UM pwm_channel_t POR CANAL */
  static pwm_channel_t pwm_channel_pin;
  PWM_init(PWM0, ID_PWM0,  &pwm_channel_pin, PWM_CHANNEL_0, 23);

  /* duty cycle */
  int duty = 0;

  /* Infinite loop */
  while (1) {
    /* fade in */
    for(duty = 0; duty <= 255; duty++){
      pwm_channel_update_duty(PWM0, &pwm_channel_pin, 255-duty);
      delay_ms(10);
    }   
    /* fade out*/
    for(duty = 0; duty <= 255; duty++){
      pwm_channel_update_duty(PWM0, &pwm_channel_pin, duty);
      delay_ms(10);
    }
  }
}
