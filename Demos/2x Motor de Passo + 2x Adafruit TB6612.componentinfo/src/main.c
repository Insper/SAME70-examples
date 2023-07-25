/**
 * 28/02/2019
 * Marco Mello
 *
 * Exemplo de utilização com:
 *
 * 2x Driver de Motor -> Adafruit TB6612 1.2A DC/Stepper Motor Driver Breakout Board
 * 2x Motor de Passo -> Mercury Motor SM-42BYG011-25
 *
 * Funcionamento:
 *
 * Os dois motores se movem simultaneamente, cada um em um sentido até completar uma volta (360º),
 * após completar uma volta, aguardam 1 segundo e giram no sentido contrário até completar outra volta.
 *
 * Conexões:
 * 
 *				Motor 1												  Motor 2
 * Adafruit TB6612 (1) - SAME70-XPLD (EXT1)				 Adafruit TB6612 (2) - SAME70-XPLD (EXT2)
 *		  PWMA		   ->		PA3								PWMA		 ->		  PA3
 *		  AIN2		   ->		PB1								AIN2		 ->		  PB1	
 *		  AIN1		   ->		PB0								AIN1		 ->		  PB0
 *		  STBY		   ->		NC*								STBY		 ->		  NC*
 *		  BIN1		   ->		PD25							BIN1		 ->		  PD25
 *		  BIN2		   ->		PD28							BIN2		 ->		  PD28
 *		  PWMB		   ->		PD20							PWMB		 ->		  PD20
 *		  GND		   ->		GND								GND			 ->		  GND
 *		  VCC		   ->		VCC								VCC		     ->		  VCC
 *		  VM		   ->		NC*								VM		     ->		  NC*
 */


// includes

#include "asf.h"

/************************************************************************/



//Defines Motor 1 (M1)

#define M1_A1			PIOB
#define M1_A1_ID        ID_PIOB
#define M1_A1_IDX       0
#define M1_A1_IDX_MASK  (1 << M1_A1_IDX)

#define M1_A2           PIOB
#define M1_A2_ID        ID_PIOB
#define M1_A2_IDX       1
#define M1_A2_IDX_MASK  (1 << M1_A2_IDX)

#define M1_B1           PIOD
#define M1_B1_ID        ID_PIOD
#define M1_B1_IDX       25
#define M1_B1_IDX_MASK  (1 << M1_B1_IDX)

#define M1_B2           PIOD
#define M1_B2_ID        ID_PIOD
#define M1_B2_IDX       28
#define M1_B2_IDX_MASK  (1 << M1_B2_IDX)

#define M1_ENA           PIOA
#define M1_ENA_ID        ID_PIOA
#define M1_ENA_IDX       3
#define M1_ENA_IDX_MASK  (1 << M1_ENA_IDX)

#define M1_ENB           PIOD
#define M1_ENB_ID        ID_PIOD
#define M1_ENB_IDX       20
#define M1_ENB_IDX_MASK  (1 << M1_ENB_IDX)

/************************************************************************/



//Defines Motor 2 (M2)
#define M2_A1			PIOA
#define M2_A1_ID        ID_PIOA
#define M2_A1_IDX       21
#define M2_A1_IDX_MASK  (1 << M2_A1_IDX)

#define M2_A2           PIOD
#define M2_A2_ID        ID_PIOD
#define M2_A2_IDX       26
#define M2_A2_IDX_MASK  (1 << M2_A2_IDX)

#define M2_B1           PIOD
#define M2_B1_ID        ID_PIOD
#define M2_B1_IDX       11
#define M2_B1_IDX_MASK  (1 << M2_B1_IDX)

#define M2_B2           PIOD
#define M2_B2_ID        ID_PIOD
#define M2_B2_IDX       30
#define M2_B2_IDX_MASK  (1 << M2_B2_IDX)

#define M2_ENA           PIOC
#define M2_ENA_ID        ID_PIOC
#define M2_ENA_IDX       13
#define M2_ENA_IDX_MASK  (1 << M2_ENA_IDX)

#define M2_ENB           PIOA
#define M2_ENB_ID        ID_PIOA
#define M2_ENB_IDX       24
#define M2_ENB_IDX_MASK  (1 << M2_ENB_IDX)

/************************************************************************/




/* prototypes                                                           */

void init(void);

/************************************************************************/



// Função de inicialização do uC
void init(void)
{
	sysclk_init();
	
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	//Motor 1 
	pmc_enable_periph_clk(M1_A1_ID);
	pio_set_output(M1_A1, M1_A1_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M1_A2_ID);
	pio_set_output(M1_A2, M1_A2_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M1_B1_ID);
	pio_set_output(M1_B1, M1_B1_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M1_B2_ID);
	pio_set_output(M1_B2, M1_B2_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M1_ENA_ID);
	pio_set_output(M1_ENA, M1_ENA_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M1_ENB_ID);
	pio_set_output(M1_ENB, M1_ENB_IDX_MASK, 1, 0, 0);
	/************************************************************************/
	
	//Motor 2	
	pmc_enable_periph_clk(M2_A1_ID);
	pio_set_output(M2_A1, M2_A1_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M2_A2_ID);
	pio_set_output(M2_A2, M2_A2_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M2_B1_ID);
	pio_set_output(M2_B1, M2_B1_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M2_B2_ID);
	pio_set_output(M2_B2, M2_B2_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M2_ENA_ID);
	pio_set_output(M2_ENA, M2_ENA_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(M2_ENB_ID);
	pio_set_output(M2_ENB, M2_ENB_IDX_MASK, 1, 0, 0);
	/************************************************************************/

}



/* Main                                                                 */
// Funcao principal chamada na inicalizacao do uC.
int main(void)
{
	
  init();

  while (1)
  {
	 
	pio_set(M1_ENA, M1_ENA_IDX_MASK); //Seta o pino PWMA
	pio_set(M1_ENB, M1_ENB_IDX_MASK); //Seta o pino PWMB
	
	pio_set(M2_ENA, M2_ENA_IDX_MASK); //Seta o pino PWMA
	pio_set(M2_ENB, M2_ENB_IDX_MASK); //Seta o pino PWMB
	
	for(int i=0; i<50; i++)
	{
		
	//Motor 1 - Passo - Fase 1 - (Sentido Horário)
	pio_set(M1_A1, M1_A1_IDX_MASK);
	pio_clear(M1_A2, M1_A2_IDX_MASK);
	pio_clear(M1_B1, M1_B1_IDX_MASK);
	pio_clear(M1_B2, M1_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 2 - Passo - Fase 4 - (Sentido Anti-Horário)
	pio_clear(M2_A1, M2_A1_IDX_MASK);
	pio_clear(M2_A2, M2_A2_IDX_MASK);
	pio_clear(M2_B1, M2_B1_IDX_MASK);
	pio_set(M2_B2, M2_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 1 - Passo - Fase 2 - (Sentido Horário)	
	pio_clear(M1_A1, M1_A1_IDX_MASK);
	pio_clear(M1_A2, M1_A2_IDX_MASK);
	pio_set(M1_B1, M1_B1_IDX_MASK);
	pio_clear(M1_B2, M1_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 2 - Passo - Fase 3 - (Sentido Anti-Horário)
	pio_clear(M2_A1, M2_A1_IDX_MASK);
	pio_set(M2_A2, M2_A2_IDX_MASK);
	pio_clear(M2_B1, M2_B1_IDX_MASK);
	pio_clear(M2_B2, M2_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 1 - Passo - Fase 3 - (Sentido Horário)	
	pio_clear(M1_A1, M1_A1_IDX_MASK);
	pio_set(M1_A2, M1_A2_IDX_MASK);
	pio_clear(M1_B1, M1_B1_IDX_MASK);
	pio_clear(M1_B2, M1_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 2 - Passo - Fase 2 - (Sentido Anti-Horário)	
	pio_clear(M2_A1, M2_A1_IDX_MASK);
	pio_clear(M2_A2, M2_A2_IDX_MASK);
	pio_set(M2_B1, M2_B1_IDX_MASK);
	pio_clear(M2_B2, M2_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 1 - Passo - Fase 4 - (Sentido Horário)	
	pio_clear(M1_A1, M1_A1_IDX_MASK);
	pio_clear(M1_A2, M1_A2_IDX_MASK);
	pio_clear(M1_B1, M1_B1_IDX_MASK);
	pio_set(M1_B2, M1_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 2 - Passo - Fase 1 - (Sentido Anti-Horário)
	pio_set(M2_A1, M2_A1_IDX_MASK);
	pio_clear(M2_A2, M2_A2_IDX_MASK);
	pio_clear(M2_B1, M2_B1_IDX_MASK);
	pio_clear(M2_B2, M2_B2_IDX_MASK);
	delay_ms(5);
	
	}
	delay_ms(1000);
	
	for(int i=0; i<50; i++)
	{
	
	//Motor 1 - Passo - Fase 4 - (Sentido Anti-Horário)	
	pio_clear(M1_A1, M1_A1_IDX_MASK);
	pio_clear(M1_A2, M1_A2_IDX_MASK);
	pio_clear(M1_B1, M1_B1_IDX_MASK);
	pio_set(M1_B2, M1_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 2 - Passo - Fase 1 - (Sentido Horário)	
	pio_set(M2_A1, M2_A1_IDX_MASK);
	pio_clear(M2_A2, M2_A2_IDX_MASK);
	pio_clear(M2_B1, M2_B1_IDX_MASK);
	pio_clear(M2_B2, M2_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 1 - Passo - Fase 3 - (Sentido Anti-Horário)	
	pio_clear(M1_A1, M1_A1_IDX_MASK);
	pio_set(M1_A2, M1_A2_IDX_MASK);
	pio_clear(M1_B1, M1_B1_IDX_MASK);
	pio_clear(M1_B2, M1_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 2 - Passo - Fase 2 - (Sentido Horário)	
	pio_clear(M2_A1, M2_A1_IDX_MASK);
	pio_clear(M2_A2, M2_A2_IDX_MASK);
	pio_set(M2_B1, M2_B1_IDX_MASK);
	pio_clear(M2_B2, M2_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 1 - Passo - Fase 2 - (Sentido Anti-Horário)	
	pio_clear(M1_A1, M1_A1_IDX_MASK);
	pio_clear(M1_A2, M1_A2_IDX_MASK);
	pio_set(M1_B1, M1_B1_IDX_MASK);
	pio_clear(M1_B2, M1_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 2 - Passo - Fase 3 - (Sentido Horário)	
	pio_clear(M2_A1, M2_A1_IDX_MASK);
	pio_set(M2_A2, M2_A2_IDX_MASK);
	pio_clear(M2_B1, M2_B1_IDX_MASK);
	pio_clear(M2_B2, M2_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 1 - Passo - Fase 1 - (Sentido Anti-Horário)	
	pio_set(M1_A1, M1_A1_IDX_MASK);
	pio_clear(M1_A2, M1_A2_IDX_MASK);
	pio_clear(M1_B1, M1_B1_IDX_MASK);
	pio_clear(M1_B2, M1_B2_IDX_MASK);
	delay_ms(5);
	
	//Motor 2 - Passo - Fase 4 - (Sentido Horário)	
	pio_clear(M2_A1, M2_A1_IDX_MASK);
	pio_clear(M2_A2, M2_A2_IDX_MASK);
	pio_clear(M2_B1, M2_B1_IDX_MASK);
	pio_set(M2_B2, M2_B2_IDX_MASK);
	delay_ms(5);
	
	}
	delay_ms(1000);	
	
  }
  
  return 0;
  
}