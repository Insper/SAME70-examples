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
 * Periféricos:
 *  - PIO
 *  - PMC
 *
 * Log:
 *  - 05/2022: Criação
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
#define SW_PIO      PIOC
#define SW_PIO_ID   ID_PIOC
#define SW_IDX		17
#define SW_IDX_MASK (1 << SW_IDX)

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

int contador = 0;

/************************************************************************/
/* prototype                                                            */
/************************************************************************/
void io_init(void);

/************************************************************************/
/* handler / callbacks                                                  */
/************************************************************************/

/*
 * callback que altera o valor do contador, aumentando-o caso o giro do encoder
 * seja no sentido horário, e diminuindo-o se o giro for anti-horário
 */
void clk_callback(void)
{	
	if (pio_get(DT_PIO, PIO_INPUT, DT_IDX_MASK) != pio_get(CLK_PIO, PIO_INPUT, CLK_IDX_MASK)) {
		contador++;
	} else {
		contador--;
	}
	
}

/************************************************************************/
/* funções                                                              */
/************************************************************************/

// Inicializa botao SW0 do kit com interrupcao
void io_init(void)
{

  // Inicializa clock dos periféricos PIOs responsaveis pelo CLK, DT e SW
    pmc_enable_periph_clk(CLK_PIO_ID);
	pmc_enable_periph_clk(DT_PIO_ID);
	pmc_enable_periph_clk(SW_PIO_ID);
	
  // ativando debounce nos pinos com frequência de 200Hz
	pio_set_debounce_filter(CLK_PIO, CLK_IDX_MASK, 200);
	pio_set_debounce_filter(DT_PIO, DT_IDX_MASK, 200);

  // Configura PIO para lidar com os pinos dos CLK, DT e SW como entrada
  // sem pullup
	pio_configure(CLK_PIO, PIO_INPUT, CLK_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(DT_PIO, PIO_INPUT, DT_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);
	pio_configure(SW_PIO, PIO_INPUT, SW_IDX_MASK, PIO_PULLUP | PIO_DEBOUNCE);

  // Configura interrupção no pino referente ao botao e associa
  // função de callback caso uma interrupção for gerada
  // a função de callback é a: but_callback()
  pio_handler_set(CLK_PIO,
                  CLK_PIO_ID,
                  CLK_IDX_MASK,
                  PIO_IT_EDGE,
                  clk_callback);
				  

  // Ativa interrupção e limpa primeira IRQ gerada na ativacao
  pio_enable_interrupt(CLK_PIO, CLK_IDX_MASK);
  pio_get_interrupt_status(CLK_PIO);
  pio_enable_interrupt(DT_PIO, DT_IDX_MASK);
  pio_get_interrupt_status(DT_PIO);
  pio_enable_interrupt(SW_PIO, SW_IDX_MASK);
  pio_get_interrupt_status(SW_PIO);
  
  // Configura NVIC para receber interrupcoes do PIO do botao
  // com prioridade 4 (quanto mais próximo de 0 maior)
  NVIC_EnableIRQ(CLK_PIO_ID);
  NVIC_SetPriority(CLK_PIO_ID, 4); // Prioridade 4
  NVIC_EnableIRQ(DT_PIO_ID);
  NVIC_SetPriority(DT_PIO_ID, 4); // Prioridade 4
  NVIC_EnableIRQ(SW_PIO_ID);
  NVIC_SetPriority(SW_PIO_ID, 4); // Prioridade 4
}


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
			if (contador > 0 && contador != contadorNovo) {
				//limpa a tela do OLED
				gfx_mono_draw_rect(0, 0, 300, 300, GFX_PIXEL_CLR);
				
				//imprime na placa OLED o sentido do giro em relação a posição original do encoder
				gfx_mono_draw_string(" horario", 0, 0, &sysfont);
				char str[128];
				
				//imprime na placa OLED o valor do contador
				sprintf(str, " %d   ", contador); 
				gfx_mono_draw_string(str, 0, 18, &sysfont);
				
				//atualiza contador novo para indicar que o valor do contador mudou
				contadorNovo = contador;
				
			}
			
			if (contador < 0 && contador != contadorNovo) {
				//limpa a tela do OLED
				gfx_mono_draw_rect(0, 0, 300, 300, GFX_PIXEL_CLR);
				
				//imprime na placa OLED o sentido do giro em relação a posição original do encoder
				gfx_mono_draw_string(" anti-horario", 0, 0, &sysfont);
				char str[128];
				
				//imprime na placa OLED o valor do contador
				sprintf(str, " %d    ", contador);
				gfx_mono_draw_string(str, 0, 18, &sysfont);
				
				//atualiza contador novo para indicar que o valor do contador mudou
				contadorNovo = contador;
			}
			
			if (contador == 0 && contador != contadorNovo) {
				//limpa a tela do OLED
				gfx_mono_draw_rect(0, 0, 300, 300, GFX_PIXEL_CLR);
				
				//imprime na placa OLED que o encoder está no estado inicial
				gfx_mono_draw_string(" inicio      ", 0, 0, &sysfont);
				char str[128];
				
				//imprime na placa OLED o valor do contador
				sprintf(str, " %d    ", contador);
				gfx_mono_draw_string(str, 0, 18, &sysfont);
				
				//atualiza contador novo para indicar que o valor do contador mudou
				contadorNovo = contador;
			}
			
	}
}
