#  Analog Comparator Controller (ACC)
Configura o comparador analógico e gera uma interrupção dependendo nas configurações do usuário.

#  Conexão e configuração
- Não é necessária

# Explicação 
![image](https://user-images.githubusercontent.com/62957465/172076354-d0085309-cc7c-4098-94a7-52673859c7b0.png)

O comparador analógico incorpora dois multiplexadores de 8 para 1 que geram duas entradas internas. Essas entradas são comparadas, resultando em uma saída de comparação. 

O DAC0 e o AD0 são selecionados como duas entradas. O usuário pode alterar a tensão de saída do DAC0 e alterar a tensão do AD0.

A tensão de saída de DAC0 varia de (1/6)*ADVREF a (5/6)*ADVREF, e a tensão de entrada de AD0 varia de 0 a ADVREF.

O evento de comparação seria gerado se a tensão de uma entrada fosse alterada pela tensão da outra entrada. Tanto eventos maiores quanto menores podem ser acionados por padrão.


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

	ul_status = acc_get_interrupt_status(ACC);

	/* Compare Output Interrupt */
	if ((ul_status & ACC_ISR_CE) == ACC_ISR_CE) {
		if (acc_get_comparison_result(ACC)) {
			puts("-ISR- Voltage Comparison Result: AD0 > DAC0\r");
		} else {
			puts("-ISR- Voltage Comparison Result: AD0 < DAC0\r");
		}
	}
```

# Referências

- Atmel Datasheet: https://www.tme.eu/Document/b85c146da65ff583bac4b0dfc098ee7b/ATSAME70-DTE.pdf

- ASF Source Code Documentation https://asf.microchip.com/docs/3.49.1/sam.drivers.acc.example.same70_xplained/html/index.html
