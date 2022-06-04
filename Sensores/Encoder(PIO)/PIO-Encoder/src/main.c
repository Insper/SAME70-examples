/************************************************************************
 * 5 semestre - Eng. da Computao - Insper
 * Tiago Seixas - tiagovs1@al.insper.edu.br
 *
 * Material:
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 *
 * Objetivo:
 *  - Exemplo de encoder usando IRQ de PIO
 *
 * Perif�ricos:
 *  - PIO
 *  - PMC
 *
 * Log:
 *  - 05/2022: Cria��o
 ************************************************************************/

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include <asf.h>

#include "gfx_mono_ug_2832hsweg04.h"
#include "gfx_mono_text.h"
#include "sysfont.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

//CLK
#define CLK_PIO     PIOC
#define CLK_PIO_ID  ID_PIOC
#define CLK_IDX		13
#define CLK_IDX_MASK (1 << CLK_IDX)

//DT
#define DT_PIO     PIOD
#define DT_PIO_ID  ID_PIOD
#define DT_IDX	   30
#define DT_IDX_MASK (1 << DT_IDX)

//SW
#define SW_PIO      PIOA
#define SW_PIO_ID   ID_PIOA
#define SW_IDX		6
#define SW_IDX_MASK (1 << SW_IDX)

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/
int contador = 0;
volatile char sw_flag = 0;

/************************************************************************/
/* prototype                                                            */
/************************************************************************/
void io_init(void);

/************************************************************************/
/* handler / callbacks                                                  */
/************************************************************************/

/*
 * callback que altera o valor do contador, aumentando-o caso o giro do encoder
 * seja no sentido hor�rio, e diminuindo-o se o giro for anti-hor�rio
 */
void clk_callback(void){	
	if (pio_get(DT_PIO, PIO_INPUT, DT_IDX_MASK) != pio_get(CLK_PIO, PIO_INPUT, CLK_IDX_MASK)) {
		contador++;
	} else {
		contador--;
	}	
}

void sw_callback(void){
	if (!pio_get(SW_PIO, PIO_INPUT, SW_IDX_MASK)) {
		sw_flag = 1;
	}
}

/************************************************************************/
/* fun��es                                                              */
/************************************************************************/

// Inicializa CLK, DT e SW do Encoder com interrup��o
void io_init(void){

	// Inicializa clock dos perif�ricos PIOs respons�veis por CLK, DT e SW
    pmc_enable_periph_clk(CLK_PIO_ID);
	pmc_enable_periph_clk(DT_PIO_ID);
	pmc_enable_periph_clk(SW_PIO_ID);
	
	// ativando debounce nos pinos com frequ�ncia de 200Hz
	pio_set_debounce_filter(CLK_PIO, CLK_IDX_MASK, 200);
	pio_set_debounce_filter(DT_PIO, DT_IDX_MASK, 200);
	pio_set_debounce_filter(SW_PIO, SW_IDX_MASK, 200);

	// Configura PIO para lidar com os pinos dos CLK, DT e SW como entrada
	// com pullup
	pio_configure(CLK_PIO, PIO_INPUT, CLK_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(DT_PIO, PIO_INPUT, DT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(SW_PIO, PIO_INPUT, SW_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);

	// Configura interrup��o no pino referente ao CLK e associa
	// a fun��o de callback caso uma interrup��o seja gerada
	// a fun��o de callback � a: clk_callback()
	pio_handler_set(CLK_PIO,
					CLK_PIO_ID,
					CLK_IDX_MASK,
					PIO_IT_RISE_EDGE,
					clk_callback);
	
	// Configura interrup��o no pino referente ao SW e associa
	// a fun��o de callback caso uma interrup��o seja gerada
	// a fun��o de callback � a: sw_callback()		
	pio_handler_set(SW_PIO,
					SW_PIO_ID,
					SW_IDX_MASK,
					PIO_IT_FALL_EDGE,
					sw_callback);
				  

	// Ativa interrup��o e limpa primeira IRQ gerada na ativacao
	pio_enable_interrupt(CLK_PIO, CLK_IDX_MASK);
	pio_get_interrupt_status(CLK_PIO);
	pio_enable_interrupt(DT_PIO, DT_IDX_MASK);
	pio_get_interrupt_status(DT_PIO);
	pio_enable_interrupt(SW_PIO, SW_IDX_MASK);
	pio_get_interrupt_status(SW_PIO);
  
	// Configura NVIC para receber interrupcoes do PIO do botao
	// com prioridade 4 (quanto mais pr�ximo de 0 maior)
	NVIC_EnableIRQ(CLK_PIO_ID);
	NVIC_SetPriority(CLK_PIO_ID, 4); // Prioridade 4
	NVIC_EnableIRQ(DT_PIO_ID);
	NVIC_SetPriority(DT_PIO_ID, 4); // Prioridade 4
	NVIC_EnableIRQ(SW_PIO_ID);
	NVIC_SetPriority(SW_PIO_ID, 4); // Prioridade 4
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/

int main (void){
	board_init();
	
	// Inicializa clock
	sysclk_init();

	// Inicializa OLED
	gfx_mono_ssd1306_init();
	
	// Configura botão com interrupção
	io_init();
	
	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	// Inicia contador de refêrencia, usado para saber se o encoder girou
	int contadorNovo = 0;

	while(1) {
		if (sw_flag == 1) {
			// Reinicia o contador
			contador = 0;
			
			// Desativa a flag
			sw_flag = 0;
		}
		
		if (contador > 0 && contador != contadorNovo) {
			// Limpa a tela do OLED
			gfx_mono_draw_rect(0, 0, 300, 300, GFX_PIXEL_CLR);
			
			// Imprime na placa OLED o sentido do giro em relação à posição original do encoder
			gfx_mono_draw_string(" horario", 0, 0, &sysfont);
			char str[128];
			
			// Imprime na placa OLED o valor do contador
			sprintf(str, " %d   ", abs(contador));
			gfx_mono_draw_string(str, 0, 18, &sysfont);
			
			// Atualiza contadorNovo para indicar que o valor do contador mudou
			contadorNovo = contador;
			
		}
		
		if (contador < 0 && contador != contadorNovo) {
			// Limpa a tela do OLED
			gfx_mono_draw_rect(0, 0, 300, 300, GFX_PIXEL_CLR);
			
			// Imprime na placa OLED o sentido do giro em relação à posição original do encoder
			gfx_mono_draw_string(" anti-horario", 0, 0, &sysfont);
			char str[128];
			
			// Imprime na placa OLED o valor do contador
			sprintf(str, " %d    ", abs(contador));
			gfx_mono_draw_string(str, 0, 18, &sysfont);
			
			// Atualiza contadorNovo para indicar que o valor do contador mudou
			contadorNovo = contador;
		}
		
		if (contador == 0 && contador != contadorNovo) {
			// Limpa a tela do OLED
			gfx_mono_draw_rect(0, 0, 300, 300, GFX_PIXEL_CLR);
			
			// Imprime na placa OLED que o encoder está no estado inicial
			gfx_mono_draw_string(" inicio      ", 0, 0, &sysfont);
			char str[128];
			
			// Imprime na placa OLED o valor do contador
			sprintf(str, " %d    ", abs(contador));
			gfx_mono_draw_string(str, 0, 18, &sysfont);
			
			// Atualiza contadorNovo para indicar que o valor do contador mudou
			contadorNovo = contador;
		}
		
	}
}