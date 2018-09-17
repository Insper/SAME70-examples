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
#define LED_PIO_PIN 8
#define LED_PIO_PIN_MASK (1 << LED_PIO_PIN)

#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_PIO_PIN 11
#define BUT_PIO_PIN_MASK (1 << LED_PIO_PIN)

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void){
	
	//board clock
	sysclk_init();
	
	//Desliga watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;	

	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(BUT_PIO_ID);
	
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_PIO_PIN_MASK, PIO_DEFAULT);
	pio_configure(BUT_PIO, PIO_INPUT, BUT_PIO_PIN_MASK, PIO_PULLUP);
	
	// super loop
	// aplicacoes embarcadas no devem sair do while(1).
	while(1){
		if(pio_get(BUT_PIO, PIO_INPUT, BUT_PIO_PIN_MASK)){
			int i = 0;
		
			for (i=0;i<5;i++){
				pio_clear(LED_PIO, LED_PIO_PIN_MASK);
				delay_ms(200);
				pio_set(LED_PIO, LED_PIO_PIN_MASK);
				delay_ms(200);  
			}
		}
		else{
			pio_set(LED_PIO, LED_PIO_PIN_MASK);
		}
	}
	return 0;
}
