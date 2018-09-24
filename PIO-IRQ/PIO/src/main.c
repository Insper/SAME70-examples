/**
 * 5 semestre - Eng. da Computao - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 * Objetivo :
 *  - Introduzir ASF e HAL
 *  - Configuracao de clock
 *  - Configuracao pino In/Out
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

#include "asf.h"


/************************************************************************/
/* defines                                                              */
/************************************************************************/

#define LED_PIO PIOC
#define LED_PIO_ID ID_PIOC
#define LED_IDX 8
#define LED_IDX_MASK (1 << LED_IDX)

#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_IDX 11
#define BUT_IDX_MASK (1 << BUT_IDX)

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* interrupcoes   / callbacks                                           */
/************************************************************************/

// exemplo de callback para o botao, sempre que acontecer
// ira piscar o led por 5 vezes
//
// ***********************************************************
// !! Isso é um exemplo ruim, nao deve ser feito na pratica, *
// !! pois nao se deve usar delays dentro de interrupcoes
//
void but_callback(void)
{
  for (int i=0;i<5;i++)
  {
    pio_clear(LED_PIO, LED_IDX_MASK);
    delay_ms(200);
    pio_set(LED_PIO, LED_IDX_MASK);
    delay_ms(200);
  }
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

// Inicializa botao do kit com interrupcao
void but_init(void)
{
  // Inicializa clock do periférico PIO responsavel pelo botao
	pmc_enable_periph_clk(BUT_PIO_ID);

  // Configura PIO para lidar com o pino do botão como entrada
  // com pull-up
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);

  // Configura interrupcao no pino referente ao botao e assoscia
  // funcao de callback caso uma interrupcao for gerada
  // a funcao de callback é a: but_callback()
  pio_handler_set(BUT_PIO,
                  BUT_PIO_ID,
                  BUT_IDX_MASK,
                  PIO_IT_FALL_EDGE,
                  but_callback);

  // Ativa interrupção
  pio_enable_interrupt(BUT_PIO, BUT_IDX_MASK);

  // Configura NVIC para receber interrupcoes do PIO do botao
  // com prioridade 4 (quanto mais proximo de 0 maior)
  NVIC_EnableIRQ(BUT_PIO_ID);
  NVIC_SetPriority(BUT_PIO_ID, 4);
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
void main(void)
{
	//board clock
	sysclk_init();

	//Desliga watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;

  // configura led
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);

  // configura botao com interrupcao
  but_init();

	// super loop
	// aplicacoes embarcadas no devem sair do while(1).
	while(1)
  {
	}
}
