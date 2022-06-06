#  Analog Comparator Controller (ACC)
Configura o comparador analógico e gera uma interrupção dependendo nas configurações do usuário.

##  Conexão e configuração
- Não é necessária

## Analog Comparator Controller - ACC

O ACC configura o Comparador Analógico e gera uma interrupção de acordo com as configurações do aplicativo do usuário. 

O comparador analógico compara duas tensões de entrada analógicas, e o resultado dessa comparação fornece uma comparação saída (emite um nível de sinal indicando qual das entradas é maior ou menor) e uma interrupção pode ser gerada de acordo. Um comparador analógico é basicamente um amplificador sem feedback e, portanto, tem ganho muito alto.

Quando as configurações do comparador analógico são modificadas, a saída da célula analógica pode ser inválida. 

Um sinalizador de comparação é acionado por um evento na saída do comparador analógico e uma interrupção é gerada.

O evento na saída do comparador analógico pode ser selecionado entre ``	falling edge``, ``rising edge`` ou ``any edge``.

O usuário pode selecionar uma alta velocidade ou opção de baixa potência, o nível de histerese, detecção de borda e polaridade.

O ACC também pode gerar um evento de comparação que pode ser usado pelo Pulse Width Modulation (PWM).

![image](https://user-images.githubusercontent.com/62957465/172083444-b36941b3-36df-4993-ab29-b3cab1f9e7a7.png)

Quando a tensão na entrada positiva (Vin0) é maior que a tensão na entrada negativa (Vin1), então a tensão de saída (VOUT) está saturada para seu alimentação positiva (+VSUPPLY), caso contrário, a saída é saturada com alimentação negativa (-VSUPPLY). Em microcontroladores, como não há tensão de alimentação negativa, GND (nível de terra) é tomado como –VSUPPLY e VCC nível é considerado +VSUPPLY.

## Diagrama de Blocos
![image](https://user-images.githubusercontent.com/62957465/172076354-d0085309-cc7c-4098-94a7-52673859c7b0.png)

O comparador analógico incorpora 8 a 1 multiplexadores em ambos suas entradas. 

O DAC0 e o AD0 são selecionados como duas entradas. O usuário pode alterar a tensão de saída do DAC0 e alterar a tensão do AD0.

A tensão de saída de DAC0 varia de (1/6)*ADVREF a (5/6)*ADVREF, e a tensão de entrada de AD0 varia de 0 a ADVREF.

O evento de comparação seria gerado se a tensão de uma entrada fosse alterada pela tensão da outra entrada. Tanto eventos maiores quanto menores podem ser acionados por padrão.

### Main

A função ``main`` desse programa é responsável por inicializar todos os periféricos envolvidos no projeto e exibir um menu para o usuário que permite definir uma nova tensão de entrada, como também pegar a voltagem do potênciometro.

```c

int main(void)
{
	uint32_t uc_key;
	int16_t s_volt = 0;
	uint32_t ul_value = 0;
	volatile uint32_t ul_status = 0x0;
	int32_t l_volt_dac0 = 0;

	/* Função para iniçialização do sistema, clocks, DACC e AFEC */
	init();
	
	/* Iniciando a entrada DAC0 em ADVREF/2  */
	dacc_write_conversion_data(DACC, MAX_DIGITAL / 2, DACC_CHANNEL_0);
	l_volt_dac0 = (MAX_DIGITAL / 2) * (2 * VOLT_REF / 3) / MAX_DIGITAL +VOLT_REF / 6;

	/* Enable clock for ACC */
	pmc_enable_periph_clk(ID_ACC);
	
	/* Inicializa o ACC*/
	acc_init(ACC, ACC_MR_SELPLUS_AFE0_AD0, ACC_MR_SELMINUS_DAC0, ACC_MR_EDGETYP_ANY, ACC_MR_INV_DIS);

	/* Enable ACC interrupt */
	NVIC_EnableIRQ(ACC_IRQn);

	/* Enable */
	acc_enable_interrupt(ACC);

	dsplay_menu(); /* Função para colocar na Terminal Window as opções do menu  */

	while (1) {
		while (usart_read(CONSOLE_UART, &uc_key)) {
		}

		printf("input: %c\r\n", uc_key);

		switch (uc_key) {
		case 's': /*s: Definir nova tensão de saída DAC0 */
		case 'S': 
			printf("Input DAC0 output voltage (%d~%d mv): ",(VOLT_REF / 6), (VOLT_REF * 5 / 6));
			s_volt = get_input_voltage(); /*Pega o valor de tensão da entrada inserida pelo usuário*/
			puts("\r");

			if (s_volt > 0) {
				l_volt_dac0 = s_volt;
				/*Coloca novo valor de tensão*/
				ul_value = ((s_volt - (VOLT_REF / 6))* (MAX_DIGITAL * 6) / 4) / VOLT_REF; 
				dacc_write_conversion_data(DACC, ul_value, DACC_CHANNEL_0); 
				puts("-I- Set ok\r");
			} else {
				puts("-I- Input voltage is invalid\r");
			}
			break;
		case 'v':  /*v: Pega a voltagem do potenciometro*/
		case 'V': 
			/* Começa conversão */
			afec_start_software_conversion(AFEC0);
			ul_status = afec_get_interrupt_status(AFEC0); /* Leitura do status do periférico */
			while ((ul_status & AFEC_ISR_EOC0) != AFEC_ISR_EOC0) {
				ul_status = afec_get_interrupt_status(AFEC0);
			}
			/* Fim da conversão */
			ul_value = afec_channel_get_value(AFEC0, AFEC_CHANNEL_POTENTIOMETER);
			
			/* Converte de AFEC data para valor em voltagem*/
			s_volt = (ul_value * VOLT_REF) / MAX_DIGITAL;
			printf("-I- Voltage on potentiometer(AD0) is %d mv\n\r", s_volt);
			printf("-I- Voltage on DAC0 is %ld mv \n\r", (long)l_volt_dac0);
			break;
			
		case 'm': /* m: Exibir este menu novamente */
		case 'M':
			dsplay_menu();
			break;
		}
	}
}

```

### Inicialização 
```c
void acc_init(Acc * p_acc, uint32_t ul_select_plus, uint32_t ul_select_minus, uint32_t ul_edge_type, uint32_t ul_invert)
 ```

### Interrupção

![image](https://user-images.githubusercontent.com/62957465/172090861-626aff4b-a012-4188-9ba5-185cfa5ff998.png)

O ACC tem uma linha de interrupção conectada ao Interrupt Controller (IC). Para lidar com interrupções, o IC deve ser programado antes de configurar o ACC.

```c
/**
 * Interrupt handler for the ACC.
 */
void ACC_Handler(void)
{
	uint32_t ul_status;

	/* Leitura do status do periférico */
	ul_status = acc_get_interrupt_status(ACC);

	/* Comparação da interrupção de saída*/
	if ((ul_status & ACC_ISR_CE) == ACC_ISR_CE) {
		if (acc_get_comparison_result(ACC)) { 
			puts("-ISR- Voltage Comparison Result: AD0 > DAC0\r");
		} else {
			puts("-ISR- Voltage Comparison Result: AD0 < DAC0\r");
		}
	}
}
```



## Referências

- Atmel Datasheet: https://www.tme.eu/Document/b85c146da65ff583bac4b0dfc098ee7b/ATSAME70-DTE.pdf

- ASF Source Code Documentation https://asf.microchip.com/docs/3.49.1/sam.drivers.acc.example.same70_xplained/html/index.html
