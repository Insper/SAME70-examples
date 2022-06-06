#  Analog Comparator Controller (ACC)
Configura o comparador analógico e gera uma interrupção dependendo nas configurações do usuário.

##  Conexão e configuração
- Não é necessária

## Diagrama de Blocos
![image](https://user-images.githubusercontent.com/62957465/172076354-d0085309-cc7c-4098-94a7-52673859c7b0.png)

O comparador analógico incorpora dois multiplexadores de 8 para 1 que geram duas entradas internas. Essas entradas são comparadas, resultando em uma saída de comparação. 

O DAC0 e o AD0 são selecionados como duas entradas. O usuário pode alterar a tensão de saída do DAC0 e alterar a tensão do AD0.

A tensão de saída de DAC0 varia de (1/6)*ADVREF a (5/6)*ADVREF, e a tensão de entrada de AD0 varia de 0 a ADVREF.

O evento de comparação seria gerado se a tensão de uma entrada fosse alterada pela tensão da outra entrada. Tanto eventos maiores quanto menores podem ser acionados por padrão.

## Analog Comparator Controller - ACC
O ACC é um módulo que compara duas tensões de entrada analógicas e emite um nível de sinal indicando qual das entradas é maior ou menor. Um comparador analógico é basicamente um amplificador sem feedback e, portanto, tem ganho muito alto.

![image](https://user-images.githubusercontent.com/62957465/172083444-b36941b3-36df-4993-ab29-b3cab1f9e7a7.png)

Quando a tensão na entrada positiva (Vin0) é maior que a tensão na entrada negativa (Vin1), então a tensão de saída (VOUT) está saturada para seu alimentação positiva (+VSUPPLY), caso contrário, a saída é saturada com alimentação negativa (-VSUPPLY). Em microcontroladores, como não há tensão de alimentação negativa, GND (nível de terra) é tomado como –VSUPPLY e VCC nível é considerado +VSUPPLY.


### Main

### Interrupção

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
