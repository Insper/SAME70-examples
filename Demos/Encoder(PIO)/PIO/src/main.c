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

#include "asf.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

//CLK
#define CLK_PIO     PIOC
#define CLK_PIO_ID  ID_PIOC
#define CLK_IDX		31
#define CLK_IDX_MASK (1 << CLK_IDX)

//DT
#define DT_PIO     PIOA
#define DT_PIO_ID  ID_PIOA
#define DT_IDX	   19
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
volatile char clk_flag;
volatile char dt_flag;
/************************************************************************/
/* prototype                                                            */
/************************************************************************/
void io_init(void);
void conta_voltas(int n);
int *numero_de_voltas;

/************************************************************************/
/* handler / callbacks                                                  */
/************************************************************************/

/*
 * Exemplo de callback para o botao, sempre que acontecer
 * ira piscar o led por 5 vezes
 *
 * !! Isso é um exemplo ruim, nao deve ser feito na pratica, !!
 * !! pois nao se deve usar delays dentro de interrupcoes    !!
 */
void clk_callback(void)
{
  if(!pio_get(CLK_PIO, PIO_INPUT, CLK_IDX_MASK)) {
	  clk_flag = 1;
  } else {
	  clk_flag = 0;
  }
  
  if(!pio_get(DT_PIO, PIO_INPUT, DT_PIO)) {
	  dt_flag = 1;
	  } else {
	  dt_flag = 0;
  }
}

/************************************************************************/
/* funções                                                              */
/************************************************************************/

// pisca led N vez no periodo T
void conta_voltas(int n){
  printf("girou %d voltas\n", n);
}

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

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
void main(void)
{
	// Inicializa clock
	sysclk_init();

	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;

  // configura botao com interrupcao
  io_init();

	// super loop
	// aplicacoes embarcadas no devem sair do while(1).
	while(1)
	{
		if (clk_flag == 1){
			if (clk_flag == dt_flag) {
				printf("girou no sentido horário");
			}
			
			if (clk_flag != dt_flag) {
				printf("girou no sentido anti-horário");
			}
		}
	}
}
