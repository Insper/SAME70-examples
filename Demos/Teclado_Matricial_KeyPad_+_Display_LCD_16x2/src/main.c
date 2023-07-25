/*
 * Data: 19/03/2019 
 * 5º Semestre - Engenharia da Computação - Insper
 * Marco Mello - marcoasma@insper.edu.br
 *
 * Exemplo de aplicação utilizando:
 *
 * 1) Teclado Matricial (KeyPad) - 16 Teclas 4x4
 * D4 -> PB0
 * D5 -> PA3
 * D6 -> PA4
 * D7 -> PB1
 * RS -> PD21
 * EN -> PD25
 *
 * 2) Display LCD 16x4 Modo 4 Bit
 * LIN1 -> PC31
 * LIN2 -> PB3
 * LIN3 -> PA0
 * LIN4 -> PD28
 * COL1 -> PC17
 * COL2 -> PC30
 * COL3 -> PB2
 * COL4 -> PA19
 *
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/
#include "asf.h"
#include "lcd_LabArqComp.h"
#include "keypad_LabArqComp.h"


/************************************************************************/
/* defines                                                              */
/************************************************************************/
#define LED_PIO           PIOC
#define LED_PIO_ID        ID_PIOC
#define LED_PIO_IDX       8
#define LED_PIO_IDX_MASK  (1 << LED_PIO_IDX)


/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void init(void);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

// Função de inicialização do uC

void init(void)
{
	sysclk_init();
	
	WDT->WDT_MR = WDT_MR_WDDIS;
		
	pmc_enable_periph_clk(LED_PIO_ID);
	pio_set_output(LED_PIO, LED_PIO_IDX_MASK, 1, 0, 0);
		
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void)
{
  init();
 
  keypad_init();	
  Lcd4_Init();
  
  int posLCDx = 0;
  int posLCDy = 0;
  
  Lcd4_Clear();
  
   // super loop
   // aplicacoes embarcadas não devem sair do while(1).
  
  while (1)
  {
	  
	pio_set(PIOC, LED_PIO_IDX_MASK); //LED da placa, disponível para utilização
	
	char lido = le_keypad(); //Chama a função de varredura do KeyPad 
	
	if (lido != '\0') //Se o que foi lido foi diferente de '\0'
	{

		if(posLCDx == 16) //Verifica se o LCD não atingiu a posição 16 em x
		{
			if(posLCDy == 1) //Se o LCD antingiu a posição 16 em x e está na segunda linha
			{
				posLCDx = 0; //Zera a posição em x
				posLCDy = 0; //Zera a posição em y
				Lcd4_Clear(); // Limpa o display
			}
			else
			{
				posLCDx = 0; //Senão, se o LCD antingiu a posição 16 em x e está na primeira linha
				posLCDy = 1; //Pula para a próxima linha do LCD definindo posLCDy = 1;
			}
		}
		Lcd4_Set_Cursor(posLCDy,posLCDx); //Posiciona o curso do LCD
		Lcd4_Write_Char(lido); //Escreve o que foi lido do KeyPad
		posLCDx++; //Incrementa a posição em x do display LCD
		
	}	
	
  }
  return 0;
}