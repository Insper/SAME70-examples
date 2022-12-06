#include <asf.h>
#include "conf_board.h"

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

/** RTOS  */
#define TASK_OLED_STACK_SIZE                (1024*6/sizeof(portSTACK_TYPE))
#define TASK_OLED_STACK_PRIORITY            (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,  signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/** prototypes */
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void PWM_init(Pwm *p_pwm, uint id_pwm, pwm_channel_t *p_channel, uint channel, uint duty);
static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback) ;
void RTT_Handler(void);
static void afec_callback(void);
static void task_motor(void *pvParameters);
void io_init(void);
void convert(uint *valor_atual, uint *valor_desejado);

void but1_callback(void);
void but2_callback(void);
void but3_callback(void);
static void BUT_init(void);

/* Include ARM math */
#include "arm_math.h"

QueueHandle_t xQueueAFEC;
QueueHandle_t xQueueMOTOR;
QueueHandle_t xQueueModo;

typedef struct {
	uint valor_atual;
	uint valor_desejado;
} potenciometro;

#define AFEC_POT AFEC0
#define AFEC_POT_ID ID_AFEC0
#define AFEC_POT_CHANNEL 0 // Canal do pino PD30

#define PIO_PWM_IN1 PIOC
#define ID_PIO_PWM_IN1 ID_PIOC
#define MASK_PIN_PWM_IN1 (1 << 19)

#define PIO_PWM_IN2 PIOA
#define ID_PIO_PWM_IN2 ID_PIOA
#define MASK_PIN_PWM_IN2 (1 << 2)

/* OLED */
#define BUT_1_PIO PIOD
#define BUT_1_PIO_ID ID_PIOD
#define BUT_1_IDX 28
#define BUT_1_IDX_MASK (1u << BUT_1_IDX)

#define BUT_2_PIO PIOC
#define BUT_2_PIO_ID ID_PIOC
#define BUT_2_IDX 31
#define BUT_2_IDX_MASK (1u << BUT_2_IDX)

#define BUT_3_PIO PIOA
#define BUT_3_PIO_ID ID_PIOA
#define BUT_3_IDX 19
#define BUT_3_IDX_MASK (1u << BUT_3_IDX)


/************************************************************************/
/* RTOS application funcs                                               */
/************************************************************************/
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed char *pcTaskName) {
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	for (;;) {	}
}

extern void vApplicationIdleHook(void) { }

extern void vApplicationTickHook(void) { }

extern void vApplicationMallocFailedHook(void) {
	configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

void RTT_Handler(void) {
	uint32_t ul_status;

	/* Get RTT status - ACK */
	ul_status = rtt_get_status(RTT);

	/* IRQ due to Alarm */
	if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
		/* Selecina canal e inicializa conversão */
		afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
		afec_start_software_conversion(AFEC_POT);
		RTT_init(1000, 100, RTT_MR_ALMIEN);
	}

}

static void afec_callback(void) {
	uint value;
	value = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xQueueSendFromISR(xQueueAFEC, &value, &xHigherPriorityTaskWoken);
}

void but1_callback(void) {
	uint32_t modo = 1;
	xQueueSendFromISR(xQueueModo, (void *)&modo, 10);
}

void but2_callback(void) {
	uint32_t modo = 2;
	xQueueSendFromISR(xQueueModo, (void *)&modo, 10);
}

void but3_callback(void) {
	uint32_t modo = 3;
	xQueueSendFromISR(xQueueModo, (void *)&modo, 10);
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_afec(void *pvParameters) {
	RTT_init(1000, 100, RTT_MR_ALMIEN);
	config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, afec_callback);
	
	gfx_mono_ssd1306_init();
	
	gfx_mono_draw_filled_rect(0, 0, 150, 20, GFX_PIXEL_CLR);
	gfx_mono_draw_string("4095", 0,0, &sysfont);

	char nome1[]="10";
	int val1 = 10;
	
	char nome2[]="2000";
	int val2 = 2000;
	
	char nome3[]="4095";
	int val3 = 4095;
	
	char *valores_potenciometro[]={&nome1, &nome2, &nome3};

	uint valor_afec;
	uint valor_oled;
	
	potenciometro pot1;
	pot1.valor_desejado = 4095;

	
	for (;;) {
		if (xQueueReceiveFromISR(xQueueAFEC, &valor_afec, 10)){
			
			pot1.valor_atual = valor_afec;
			
			
		} 
		
		if (xQueueReceiveFromISR(xQueueModo, &valor_oled, 10)){
			gfx_mono_draw_filled_rect(0, 0, 150, 20, GFX_PIXEL_CLR);
			gfx_mono_draw_string("entrou", 0,0, &sysfont);
			char *valor_potenciometro = valores_potenciometro[valor_oled-1];
			
			gfx_mono_draw_filled_rect(0, 0, 150, 20, GFX_PIXEL_CLR);
			gfx_mono_draw_string(valor_potenciometro, 0,0, &sysfont);
			
			if (valor_oled == 1){
				pot1.valor_desejado = val1;
			}
			if (valor_oled == 2){
				pot1.valor_desejado = val2;
			}
			if (valor_oled == 3){
				pot1.valor_desejado = val3;
			}
		}
		
		xQueueSend(xQueueMOTOR, (void *)&pot1, 10);

	}
}

static void task_motor(void *pvParameters) {
	pmc_enable_periph_clk(ID_PIO_PWM_IN1);
	pio_set_peripheral(PIO_PWM_IN1, PIO_PERIPH_B, MASK_PIN_PWM_IN1);
	static pwm_channel_t pwm_channel_pin_in1;
	PWM_init(PWM0, ID_PWM0,  &pwm_channel_pin_in1, PWM_CHANNEL_2, 23);
	
	pmc_enable_periph_clk(ID_PIO_PWM_IN2);
	pio_set_peripheral(PIO_PWM_IN2, PIO_PERIPH_A, MASK_PIN_PWM_IN2);
	static pwm_channel_t pwm_channel_pin_in2;
	PWM_init(PWM0, ID_PWM0,  &pwm_channel_pin_in2, PWM_CHANNEL_1, 23);
	
	potenciometro pot2;
	
	/* PID error */
	float pid_error;
	
	float duty;
	arm_pid_instance_f32 PID;
	
	/* Set PID parameters */
	/* Set this for your needs */
	PID.Kp = 1000;        /* Proporcional */
	PID.Ki = 0.05;        /* Integral */
	PID.Kd = 0.1;        /* Derivative */
	
	
	/* Initialize PID system, float32_t format */
	arm_pid_init_f32(&PID, 1);
	
	int debug_pot;

	while (1) {
		if (xQueueReceive(xQueueMOTOR, &pot2, 10)) {
			debug_pot = 1;		
		}
		
		
		/* Calculate error */
		convert(&pot2.valor_atual, &pot2.valor_desejado);
		pid_error = pot2.valor_desejado - pot2.valor_atual;
		
		/* Calculate PID here, argument is error */
		/* Output data will be returned, we will use it as duty cycle parameter */
		
		duty = arm_pid_f32(&PID, pid_error);
		
		
		/* Check overflow, duty cycle in percent */
		if (duty > 255) {
			duty = 255;
			} else if (duty < 0) {
			duty = 0;
		}
		
		if (pot2.valor_atual  + 20 < pot2.valor_desejado) {
			pwm_channel_update_duty(PWM0, &pwm_channel_pin_in1, duty);
			pwm_channel_update_duty(PWM0, &pwm_channel_pin_in2, 0);
		} else if (pot2.valor_atual - 20 > pot2.valor_desejado) {
			pwm_channel_update_duty(PWM0, &pwm_channel_pin_in1, 0);
			pwm_channel_update_duty(PWM0, &pwm_channel_pin_in2, duty);
		} else {
			pwm_channel_update_duty(PWM0, &pwm_channel_pin_in1, 0);
			pwm_channel_update_duty(PWM0, &pwm_channel_pin_in2, 0);
		}

	}
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void convert(uint *valor_atual, uint *valor_desejado) {
	*valor_atual = *valor_atual * 255/4095;
	*valor_desejado = *valor_desejado * 255/4095;
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

static void RTT_init(float freqPrescale, uint32_t IrqNPulses,
uint32_t rttIRQSource) {

	uint16_t pllPreScale = (int)(((float)32768) / freqPrescale);

	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);

	if (rttIRQSource & RTT_MR_ALMIEN) {
		uint32_t ul_previous_time;
		ul_previous_time = rtt_read_timer_value(RTT);
		while (ul_previous_time == rtt_read_timer_value(RTT))
		;
		rtt_write_alarm_time(RTT, IrqNPulses + ul_previous_time);
	}

	/* config NVIC */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 4);
	NVIC_EnableIRQ(RTT_IRQn);

	/* Enable RTT interrupt */
	if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
	rtt_enable_interrupt(RTT, rttIRQSource);
	else
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
}

void PWM_init(Pwm *p_pwm, uint id_pwm, pwm_channel_t *p_channel, uint channel, uint duty){
	
	/* Enable PWM peripheral clock */
	pmc_enable_periph_clk(id_pwm);

	/* Disable PWM channels for LEDs */
	pwm_channel_disable(p_pwm, channel);

	/* Set PWM clock A as PWM_FREQUENCY*PERIOD_VALUE (clock B is not used) */
	pwm_clock_t clock_setting = {
		.ul_clka = 4000 * 255,
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
	p_channel->ul_period = 255;
	/* Duty cycle value of output waveform */
	p_channel->ul_duty = duty;
	p_channel->channel = channel;
	pwm_channel_init(p_pwm, p_channel);
	
	/* Enable PWM channels for LEDs */
	pwm_channel_enable(p_pwm, channel);
}

void io_init(void) {
	pmc_enable_periph_clk(BUT_1_PIO_ID);
	pmc_enable_periph_clk(BUT_2_PIO_ID);
	pmc_enable_periph_clk(BUT_3_PIO_ID);

	pio_configure(BUT_1_PIO, PIO_INPUT, BUT_1_IDX_MASK, PIO_PULLUP| PIO_DEBOUNCE);
	pio_configure(BUT_2_PIO, PIO_INPUT, BUT_2_IDX_MASK, PIO_PULLUP| PIO_DEBOUNCE);
	pio_configure(BUT_3_PIO, PIO_INPUT, BUT_3_IDX_MASK, PIO_PULLUP| PIO_DEBOUNCE);

	pio_handler_set(BUT_1_PIO, BUT_1_PIO_ID, BUT_1_IDX_MASK, PIO_IT_FALL_EDGE,
	but1_callback);
	pio_handler_set(BUT_2_PIO, BUT_2_PIO_ID, BUT_2_IDX_MASK, PIO_IT_FALL_EDGE,
	but2_callback);
	pio_handler_set(BUT_3_PIO, BUT_3_PIO_ID, BUT_3_IDX_MASK, PIO_IT_FALL_EDGE,
	but3_callback);

	pio_enable_interrupt(BUT_1_PIO, BUT_1_IDX_MASK);
	pio_enable_interrupt(BUT_2_PIO, BUT_2_IDX_MASK);
	pio_enable_interrupt(BUT_3_PIO, BUT_3_IDX_MASK);

	pio_get_interrupt_status(BUT_1_PIO);
	pio_get_interrupt_status(BUT_2_PIO);
	pio_get_interrupt_status(BUT_3_PIO);

	NVIC_EnableIRQ(BUT_1_PIO_ID);
	NVIC_SetPriority(BUT_1_PIO_ID, 4);

	NVIC_EnableIRQ(BUT_2_PIO_ID);
	NVIC_SetPriority(BUT_2_PIO_ID, 4);

	NVIC_EnableIRQ(BUT_3_PIO_ID);
	NVIC_SetPriority(BUT_3_PIO_ID, 4);
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/

int main(void) {
	/* Initialize the SAM system */
	sysclk_init();
	board_init();
	io_init();

	/* Initialize the console uart */
	configure_console();
	
	xQueueAFEC = xQueueCreate(32, sizeof(uint32_t));
	if (xQueueAFEC == NULL)
	printf("falha em criar a queue \n");
	
	xQueueMOTOR = xQueueCreate( 32, sizeof(potenciometro));
	if (xQueueMOTOR == NULL)
	printf("falha em criar a queue \n");
	
	xQueueModo = xQueueCreate( 32, sizeof(uint32_t));
	if (xQueueModo == NULL)
	printf("falha em criar a queue \n");

	/* Create task to control oled */
	if (xTaskCreate(task_motor, "motor", TASK_OLED_STACK_SIZE, NULL,
	TASK_OLED_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create task_motor\r\n");
	}
	
	if (xTaskCreate(task_afec, "led", TASK_OLED_STACK_SIZE, NULL,
	TASK_OLED_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create task_afec\r\n");
	}

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* RTOS não deve chegar aqui !! */
	while(1){}

	/* Will only get here if there was insufficient memory to create the idle task. */
	return 0;
}
