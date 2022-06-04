# Encoder (PIO) - IRQ

Configura o encoder para modificar o numero de voltas dependendo de que direção ele foi girado

## Conexão e configuração

- Conectar o pino GND do encoder no pino GND da placa
- Conectar o pino + do encoder no pino 3V3 da placa
- Conectar o pino CLK do encoder no pino PC13 da placa
- Conectar o pino DT do encoder no pino PD30 da placa
- Conectar o pino SW do encoder no pino PA6 da placa
- Conectar a placa OLED no conjunto de pinos EXT1 da placa

# Encoder PIO - IRQ

Quando o encoder é girado, duas ondas quadradas são produzidas, com seu output em CLK e DT. Quando se gira no sentido horário, a primeira onda é produzida pelo CLK, e na metado do tempo em que CLK permanece em pico, a onda do DT é então produzida, as duas ficando em pico simultaneamente por um quarto de período, quando então a onda de CLK desce para seu valor mínimo (0) enquanto DT continua em pico.

Para melhor ilustrar isso, veja a imagem a seguir:

![alt text](https://github.com/TiagoSeixas2103/SAME70-examples/blob/master/Sensores/Encoder(PIO)/squareGraphs.png)

Caso o encoder seja girado no sentido antihorário, a onda produzida primeiro será a de DT, seguida da onda produzida por CLK.

# Explicação

Como só precisamos comparar as ondas produzidas por CLK e DT uma vez para saber qual foi a primeira onda produzida, nós só precisamos fazer uma função de callback, que checa no momento em que houve o giro se as duas ondas estão em pico ou não. Por conveniência, nós usamos
o output do CLK, que é produzido primeiro quando giramos o encoder para o sentido horário.

Como comparamos as duas ondas no momento em que CLK chega no seu pico, isso significa que quando CLK = DT, a onda DT foi a primeira a ser produzida, logo o encoder foi girado no sentido anti-horário. Caso CLK != DT no momento em que CLK atingir seu pico, então CLK foi a primeira onda a ser produzida, e portanto o encoder foi girado no sentido horário.

## Definindo as funções de callback

Como mencionado antes, só uma função de callback foi definida para comparar as saídas de DT e CLK, e ela varia um contador que contém o valor de voltas que o encoder deu em relação a sua posição inicial, ou seja, se o encoder for girado 10 vezes no sentido horário e uma vez no sentido anti-horário, o valor do contador será 9, pois o encoder está rotacionado 9 voltas no sentido horário em relação ao seu estado inicial. Caso fossem 10 vezes no sentido anti-horário e uma vez no sentido horário, o valor do contador seria -10. Essa função será chamada sempre que ocorrer uma rotação do encoder.

```c
/************************************************************************/
/* handler / callbacks                                                  */
/************************************************************************/

/*
 * callback que altera o valor do contador, aumentando-o caso o giro do encoder
 * seja no sentido horário, e diminuindo-o se o giro for anti-horário
 */

void clk_callback(void){	
	if (pio_get(DT_PIO, PIO_INPUT, DT_IDX_MASK) != pio_get(CLK_PIO, PIO_INPUT, CLK_IDX_MASK)) {
		contador++;
	} else {
		contador--;
	}	
}
```

Além disso, também há uma função de callback que é usada para detectar se o encoder foi pressionado, ou seja, se a saída SW gerou sinal. Sempre que o Encoder for pressionado, a flag sw_flag será ativada, indicando a ação.

```c

void sw_callback(void){
	if (!pio_get(SW_PIO, PIO_INPUT, SW_IDX_MASK)) {
		sw_flag = 1;
	}
}

```

# Definindo io_init

A função a seguir configura a placa para poder utilizar as saídas CLK, DT e SW, além de configurar os callbacks mostrados.

```c

/************************************************************************/
/* prototype                                                            */
/************************************************************************/
void io_init(void);

/************************************************************************/
/* funções                                                              */
/************************************************************************/

// Inicializa CLK, DT e SW do Encoder com interrup��o
void io_init(void){

	// Inicializa clock dos periféricos PIOs responsáveis por CLK, DT e SW
    pmc_enable_periph_clk(CLK_PIO_ID);
	pmc_enable_periph_clk(DT_PIO_ID);
	pmc_enable_periph_clk(SW_PIO_ID);
	
	// ativando debounce nos pinos com frequência de 200Hz
	pio_set_debounce_filter(CLK_PIO, CLK_IDX_MASK, 200);
	pio_set_debounce_filter(DT_PIO, DT_IDX_MASK, 200);
	pio_set_debounce_filter(SW_PIO, SW_IDX_MASK, 200);

	// Configura PIO para lidar com os pinos dos CLK, DT e SW como entrada
	// com pullup
	pio_configure(CLK_PIO, PIO_INPUT, CLK_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(DT_PIO, PIO_INPUT, DT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(SW_PIO, PIO_INPUT, SW_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);

	// Configura interrupção no pino referente ao CLK e associa
	// a função de callback caso uma interrupção seja gerada
	// a função de callback é a: clk_callback()
	pio_handler_set(CLK_PIO,
					CLK_PIO_ID,
					CLK_IDX_MASK,
					PIO_IT_RISE_EDGE,
					clk_callback);
	
	// Configura interrupção no pino referente ao SW e associa
	// a função de callback caso uma interrupção seja gerada
	// a função de callback é a: sw_callback()		
	pio_handler_set(SW_PIO,
					SW_PIO_ID,
					SW_IDX_MASK,
					PIO_IT_FALL_EDGE,
					sw_callback);
				  

	// Ativa interrupção e limpa primeira IRQ gerada na ativação
	pio_enable_interrupt(CLK_PIO, CLK_IDX_MASK);
	pio_get_interrupt_status(CLK_PIO);
	pio_enable_interrupt(DT_PIO, DT_IDX_MASK);
	pio_get_interrupt_status(DT_PIO);
	pio_enable_interrupt(SW_PIO, SW_IDX_MASK);
	pio_get_interrupt_status(SW_PIO);
  
	// Configura NVIC para receber interrupções do PIO do botão
	// com prioridade 4 (quanto mais próximo de 0 maior)
	NVIC_EnableIRQ(CLK_PIO_ID);
	NVIC_SetPriority(CLK_PIO_ID, 4); // Prioridade 4
	NVIC_EnableIRQ(DT_PIO_ID);
	NVIC_SetPriority(DT_PIO_ID, 4); // Prioridade 4
	NVIC_EnableIRQ(SW_PIO_ID);
	NVIC_SetPriority(SW_PIO_ID, 4); // Prioridade 4
}
```

# Definindo main

Na função main que se realiza os prints no OLED, informando o módulo do contador e em qual sentido o encoder se encontra rotacionado, seja horário ou anti-horário, e caso o valor do contador seja 0, aparece no visor do OLED a palavra inicio. Nesse módulo, a saída SW do encoder, que indica se o mesmo foi pressionado, reinicia o contador para 0 apenas para demonstrar como usar essa saída.

No código a seguir vemos as variáveis globais usadas na função main sendo definidas.

```c
/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/
int contador = 0;
volatile char sw_flag = 0;

```



```c

/************************************************************************/
/* main                                                                 */
/************************************************************************/

int main (void)
{
	board_init();
	
	// Inicializa clock
	sysclk_init();

	// Init OLED
	gfx_mono_ssd1306_init();
  
	// configura botao com interrupcao
	io_init();
  
	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
  
	//inicia contador de refêrencia, usado para saber se o encoder girou
	int contadorNovo = 0;

	/* Insert application code here, after the board has been initialized. */
	while(1) {
			if (sw_flag == 1) {
				contador = 0;
				//imprime na placa OLED o valor do contador
				gfx_mono_draw_string("r", 0, 18, &sysfont);
				sw_flag = 0;
			}
			
			if (contador > 0 && contador != contadorNovo) {
				//limpa a tela do OLED
				gfx_mono_draw_rect(0, 0, 300, 300, GFX_PIXEL_CLR);
				
				//imprime na placa OLED o sentido do giro em rela��o a posi��o original do encoder
				gfx_mono_draw_string(" horario", 0, 0, &sysfont);
				char str[128];
				
				//imprime na placa OLED o valor do contador
				sprintf(str, " %d   ", abs(contador)); 
				gfx_mono_draw_string(str, 0, 18, &sysfont);
				
				//atualiza contador novo para indicar que o valor do contador mudou
				contadorNovo = contador;
				
			}
			
			if (contador < 0 && contador != contadorNovo) {
				//limpa a tela do OLED
				gfx_mono_draw_rect(0, 0, 300, 300, GFX_PIXEL_CLR);
				
				//imprime na placa OLED o sentido do giro em rela��o a posi��o original do encoder
				gfx_mono_draw_string(" anti-horario", 0, 0, &sysfont);
				char str[128];
				
				//imprime na placa OLED o valor do contador
				sprintf(str, " %d    ", abs(contador));
				gfx_mono_draw_string(str, 0, 18, &sysfont);
				
				//atualiza contador novo para indicar que o valor do contador mudou
				contadorNovo = contador;
			}
			
			if (contador == 0 && contador != contadorNovo) {
				//limpa a tela do OLED
				gfx_mono_draw_rect(0, 0, 300, 300, GFX_PIXEL_CLR);
				
				//imprime na placa OLED que o encoder est� no estado inicial
				gfx_mono_draw_string(" inicio      ", 0, 0, &sysfont);
				char str[128];
				
				//imprime na placa OLED o valor do contador
				sprintf(str, " %d    ", abs(contador));
				gfx_mono_draw_string(str, 0, 18, &sysfont);
				
				//atualiza contador novo para indicar que o valor do contador mudou
				contadorNovo = contador;
			}
			
	}
}

```
