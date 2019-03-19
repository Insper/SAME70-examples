#include <asf.h>


#define POT_AFEC         AFEC0
#define POT_AFEC_ID      ID_AFEC0
#define POT_AFEC_CH      AFEC_CHANNEL_5 // Pin PB2
#define POT_AFEC_CH_IR   AFEC_INTERRUPT_EOC_5

#define IN1_M1_A			PIOD
#define IN1_M1_A_ID			ID_PIOD
#define IN1_M1_A_IDX		28
#define IN1_M1_A_IDX_MASK (1 << IN1_M1_A_IDX)

#define IN2_M1_B			PIOA
#define IN2_M1_B_ID			ID_PIOA
#define IN2_M1_B_IDX		0
#define IN2_M1_B_IDX_MASK (1 << IN2_M1_B_IDX)

#define IN3_VERD			PIOC
#define IN3_VERD_ID			ID_PIOC
#define IN3_VERD_IDX		17
#define IN3_VERD_IDX_MASK (1 << IN3_VERD_IDX)

#define IN4_VERM			PIOC
#define IN4_VERM_ID			ID_PIOC
#define IN4_VERM_IDX		30
#define IN4_VERM_IDX_MASK (1 << IN4_VERM_IDX)

/** Flag para confirmação de conversão */
volatile bool is_conversion_done = false;

/** Variável para armazenar valor da conversão */
volatile uint32_t g_ul_value = 0;

static void configure_console(void){
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

static void afec_pot_end_conversion(void){
	g_ul_value = afec_channel_get_value(POT_AFEC, POT_AFEC_CH);
	is_conversion_done = true;
}

void pot_init(void){
	pmc_enable_periph_clk(POT_AFEC_ID);
	afec_enable(POT_AFEC);
	struct afec_config afec_cfg;
	afec_get_config_defaults(&afec_cfg);
	afec_init(POT_AFEC, &afec_cfg);
	afec_set_trigger(POT_AFEC, AFEC_TRIG_SW);

	struct afec_ch_config afec_ch_cfg;
	afec_ch_get_config_defaults(&afec_ch_cfg);
	afec_ch_cfg.gain = AFEC_GAINVALUE_0;
	afec_ch_set_config(POT_AFEC, POT_AFEC_CH, &afec_ch_cfg);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_5, 0x200); // internal ADC offset is 0x200, it should cancel it and shift to 0
	afec_set_callback(POT_AFEC, POT_AFEC_CH_IR, afec_pot_end_conversion, 1);
	afec_channel_enable(POT_AFEC, POT_AFEC_CH);
	NVIC_SetPriority(AFEC0_IRQn, 10);
}

void pot_enable_interrupt(void){
	afec_enable_interrupt(POT_AFEC, POT_AFEC_CH_IR);
	NVIC_EnableIRQ(AFEC0_IRQn);
}

void pot_convert(void){
	if(!is_conversion_done){
		afec_start_software_conversion(AFEC0);
	}
}

void init(){
	
	sysclk_init();
	
	board_init();
	
	configure_console();
	
	pmc_enable_periph_clk(IN1_M1_A_ID);
	pio_set_output(IN1_M1_A, IN1_M1_A_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(IN2_M1_B_ID);
	pio_set_output(IN2_M1_B, IN2_M1_B_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(IN3_VERD_ID);
	pio_set_output(IN3_VERD, IN3_VERD_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(IN4_VERM_ID);
	pio_set_output(IN4_VERM, IN4_VERM_IDX_MASK, 1, 0, 0);
	
	pio_clear(IN1_M1_A, IN1_M1_A_IDX_MASK); //Desaciona Motor M1_A
	pio_clear(IN2_M1_B, IN2_M1_B_IDX_MASK); //Desaciona Motor M1_B
	pio_set(IN3_VERD, IN3_VERD_IDX_MASK); //Desaciona a cor VERDE da fita
	pio_set(IN4_VERM, IN4_VERM_IDX_MASK); //Desaciona a cor VERMELHA da fita
	
	pot_init(); //Chama função de inicialização do potenciometro (AFEC0 / CHANNEL5) -> Pino PB2 
	pot_enable_interrupt(); //Habilita interrupção para a leitura analógica do AFEC0
	
	
}



int main (void)
{
	init();

	uint32_t g_percentage = 0;

	while(1){
		
		pot_convert();
		
		if(is_conversion_done){
			g_percentage = (100*g_ul_value) / 4096; //Converte o valor lido em porcentagem
			printf("temp 0 : %d\r\n", g_percentage);
			is_conversion_done = false;
			}
		
		if(g_percentage>51){
			//Gira o Motor 1 no sentido horario
			pio_set(IN1_M1_A, IN1_M1_A_IDX_MASK);
			pio_clear(IN2_M1_B, IN2_M1_B_IDX_MASK);
			//=================================
			
			//Desaciona cor VERDE e aciona cor VERMELHO
			pio_set(IN3_VERD, IN3_VERD_IDX_MASK);
			pio_clear(IN4_VERM, IN4_VERM_IDX_MASK);
			//=================================
			delay_us(500);
		}
		
		else if (g_percentage<49){
			//Gira o Motor 1 no sentido anti-horario
			pio_clear(IN1_M1_A, IN1_M1_A_IDX_MASK);
			pio_set(IN2_M1_B, IN2_M1_B_IDX_MASK);
			//=================================
			
			//Desaciona cor VERDE e aciona cor VERMELHO
			pio_set(IN3_VERD, IN3_VERD_IDX_MASK);
			pio_clear(IN4_VERM, IN4_VERM_IDX_MASK);
			//=================================
			delay_us(500);
		}
		
		else{
			//Desaciona cor VERMELHO e aciona cor VERDE
			pio_clear(IN3_VERD, IN3_VERD_IDX_MASK);
			pio_set(IN4_VERM, IN4_VERM_IDX_MASK);
			//=================================
		}
		
		//Desaciona Motor
		pio_clear(IN1_M1_A, IN1_M1_A_IDX_MASK);
		pio_clear(IN2_M1_B, IN2_M1_B_IDX_MASK);
		//=================================
	}
}
