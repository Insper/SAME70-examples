# Potenciômetro Motorizado (PWM - PID)

Muda a resistência do potênciometro usando um motor para chegar ao valor de resistência determinado ao apertar o botão da placa OLED, e por meio de PWM (Pulse Width Modulation) normalizada por um controlador PID (leva em consideração os ganhos proporcional, integrativo e derivativo).

## Conexão e configuração da Ponte H, do Potenciômetro e da Placa Atmel

![alt text](https://github.com/TiagoSeixas2103/22b-emb-af-TiagoSeixas2103/blob/main/Ponte_H.png)

![alt text](https://github.com/TiagoSeixas2103/22b-emb-af-TiagoSeixas2103/blob/main/Motor_potenciometro.jpeg)

![alt text](https://github.com/TiagoSeixas2103/22b-emb-af-TiagoSeixas2103/blob/main/Ptenciometro.jpeg)

- Conectar o pino OUT1 do Motor A da ponte h no pino positivo do motor do potenciometro (sticker vermelho)

- Conectar o pino OUT2 do Motor A da ponte h no pino negativo do motor do potenciometro

- Conectar o pino 3 do potenciometro no pino 3v3 da placa Atmel 

- Conectar o pino 2 do potenciometro no pino PD30 da placa Atmel

- Conectar o pino 1 do potenciometro no pino GND da placa Atmel

- Conectar o pino GND da ponte h no pino GND da placa Atmel

- Conectar o pino 6~35V da ponte h em uma fonte DC (ou na saída 5V da placa Atmel se uma fonte DC não for fornecida)

- Conectar o pino IN1 da ponte h no pino PC19 da placa Atmel

- Conectar o pino IN2 da ponte h no pino PA2 da placa Atmel

- Conectar placa OLED no EXT1 da placa Atmel

# Explicação

O exemplo utiliza do valor atual do potenciômetro, fornecido pela saída 2 do mesmo.

O exmeplo utiliza dos valores predeterminados para serem selecionados pela placa OLED, ao pressionar diferentes botões da mesma.

O exemplo usa as entradas IN1 e IN2 da ponte h para ligar o motor do potenciômetro quando o valor atual do mesmo diferir do valor desejado, com o IN1 atuando como polo positivo da fonte e o IN2 como terra quando o valor desejado é maior do que o valor atual, e o contrário quando valor desejado é menor do que o valor atual.

# Potenciometro motorizado - (PWM - PID)

Se utiliza do valor atual do potenciômetro e o valor solicitado ao pressionar um dos botões da placa OLED para saber qual o erro do PID (diferença entre valor atual e valor desejado). Caso o erro seja muito grande, o motor então direciona a resistividade do potenciômetro para o valor desejado por meio de pulsos gerados pelo PWM, cujo duty é determinado pelo controle PID.

# Definindo as funções de callback e handlers

Foram utilizadas quatro funções de callback e um handler no handout. 

O handler para rtt foi utilizado de forma a ativar o afec de forma periódica, para assim termos a todos os momentos o valor atual do potenciômetro. 

```c
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
```

Três dos callbacks foram usados para determinar por interrupção qual botão da placa OLED foi selecionado, e portanto, qual o valor desejado para o potenciômetro, sendo esse "modo" enviado para a fila xQueueModo.

```c
QueueHandle_t xQueueModo;

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
```

Por fim, o último callback foi utilizado para ler o valor atual do potenciômetro, e enviá-los para a fila xQueueAFEC.

```c
QueueHandle_t xQueueAFEC;

#define AFEC_POT AFEC0
#define AFEC_POT_ID ID_AFEC0
#define AFEC_POT_CHANNEL 0 // Canal do pino PD30

static void afec_callback(void) {
	uint value;
	value = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
	BaseType_t xHigherPriorityTaskWoken = pdTRUE;
	xQueueSendFromISR(xQueueAFEC, &value, &xHigherPriorityTaskWoken);
}
```

# Definindo as tasks

Foram utilizadas duas tasks nesse módulo. A task_afec mexe com os valores de afec e com o OLED, e a task_motor usa os valores recebidos da task_afec pela fila xQueueMOTOR para alterar o valor do potenciômetro para o valor desejado usando PWM controlado por PID. 

Na task_afec, primeiro se inicializa o rtt e a tela daplaca OLED, já escrevendo o valor 4095, que é o valor determinado como default do valor desejado para o potenciômetro. Em seguida, foram determinados os valores controlados pelos botões da placa OLED que serão os valores alvos para os quais o potenciômetro terá que se ajustar. Por fim, foi feito um loop infinito, onde o valor lido pelo afec é armazenado em uma variável de tipo struct potenciometro (em valor_atual), o modo determinado pelo botão pressionado é usado para determinar o valor armazenado na mesma variável de tipo struct potenciometro (em valor_desejado), e por fim a variável de tipo struct é enviada pela fila xQueueMOTOR para a task_motor.

```c
QueueHandle_t xQueueMOTOR;

typedef struct {
	uint valor_atual;
	uint valor_desejado;
} potenciometro;

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
```

Na task_motor, primeiro se inicializa os PWMs para os pinos IN1 e IN2 da ponte h, e prepara os parâmetros do controlador PID. Em seguida, se inicializa o loop infinito, onde se recebe os valores atual e desejado pela fila xQueueMOTOR, que são então convertidos de uma escala (0-4095) para outra (0-255) usando a função convert. Em seguida é calculado o erro do pid (valor atual - valor desejado) e, usando a função arm_pid_f32, calcula-se o duty para o PWM.

```c
#define PIO_PWM_IN1 PIOC
#define ID_PIO_PWM_IN1 ID_PIOC
#define MASK_PIN_PWM_IN1 (1 << 19)

#define PIO_PWM_IN2 PIOA
#define ID_PIO_PWM_IN2 ID_PIOA
#define MASK_PIN_PWM_IN2 (1 << 2)

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
```

# Definindo funções e bibliotecas

Seguem os protótipos das funções usadas pelo módulo. Para checar a implementação de cada uma, abra o módulo no microchip studio e selecione a função, cilque no botão direito do mouse e selecione Goto Implementation.

```c
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

/* prototype                                                            */

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
void PWM_init(Pwm *p_pwm, uint id_pwm, pwm_channel_t *p_channel, uint channel, uint duty);
static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback) ;
void RTT_Handler(void);
static void afec_callback(void);
void io_init(void);
void convert(uint *valor_atual, uint *valor_desejado);
static void BUT_init(void);

/* Include ARM math */
#include "arm_math.h"
```

# Definindo main

No main só são criadas as filas e as tasks e inicializadas algumas funções utilizadas, pois como este módulo foi utilizando RTOS, não há necessidade de usar o loop do main.

```c
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
```

## Comentário Importante

Não foi usado nenhum printf nesse módulo por um motivo: ele atrapalha o pwm na hora de parar o potenciômetro. Não use o printf, pois ele pode causar oscilações intermitentes no potenciômetro. O mesmo pode ocorrer devido a mau contato dos fios.

# Vídeo de explicação no Youtube

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/A9g79GdGBsA/0.jpg)](https://www.youtube.com/watch?v=A9g79GdGBsA)