/************************************************************************
* 5 semestre - Eng. da Computao - Insper
* Rafael Corsi - rafael.corsi@insper.edu.br
*
* Material:
*  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
*
* Objetivo:
*  - Demonstrar configuraçao do PIO
*
* Periféricos:
*  - PIO
*  - PMC
*
* Log:
*  - 10/2018: Criação
************************************************************************/

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"
/************************************************************************/
/* defines                                                              */
/************************************************************************/

// LEDs
#define LED_PIO      PIOC
#define LED_PIO_ID   ID_PIOC
#define LED_IDX      8
#define LED_IDX_MASK (1 << LED_IDX)

// PIN_DATA
#define PIN_DATA      PIOD
#define PIN_DATA_ID   ID_PIOD
#define PIN_DATA_IDX  30
#define PIN_DATA_IDX_MASK (1 << PIN_DATA_IDX)

// Botão
#define BUT_PIO      PIOA
#define BUT_PIO_ID   ID_PIOA
#define BUT_IDX      11
#define BUT_IDX_MASK (1 << BUT_IDX)

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_BLUE 2
#define COLOR_WHITE 3
#define COLOR_BLACK 4

#define RED 100,0,0
#define GREEN 0,100,0
#define BLUE 0,0,100

#include "tipoDado.h"
#include "cores_letras.h"


/************************************************************************/
/* prototypes                                                           */
/************************************************************************/


/************************************************************************/
/* constants                                                            */
/************************************************************************/
uint32_t NOME = 0xFF00;
/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/






/************************************************************************/
/* funções                                                              */
/************************************************************************/

/************************************************************************/
/* Main                                                                 */
/************************************************************************/
/* Funcao principal chamada na inicalizacao do uC.                      */




int main(void)
{
	// Inicializa clock
	sysclk_init();

	// Desativa watchdog
	WDT->WDT_MR = WDT_MR_WDDIS;

  // Ativa PIOs
	pmc_enable_periph_clk(LED_PIO_ID);
	pmc_enable_periph_clk(BUT_PIO_ID);

  // Configura Pinos
	pio_configure(LED_PIO, PIO_OUTPUT_0, LED_IDX_MASK, PIO_DEFAULT);
	pio_configure(BUT_PIO, PIO_INPUT, BUT_IDX_MASK, PIO_PULLUP);

	// Configura led
	pmc_enable_periph_clk(PIN_DATA_ID);
	pio_configure(PIN_DATA, PIO_OUTPUT_0, PIN_DATA_IDX_MASK, PIO_DEFAULT);

	
	clearLEDs();
	//char bufferNOME[8] = {'V','I','N','I','-','-','-','-'};
	char bufferNOME[8] = {'B','E','M','V','I','N','D','O'};
	//char bufferNOME[8] = {'M','A','R','C','O','-','-','-'};
				
	while(1){
	
	int8_t size=0;
	for(int8_t i=0;i<=7;i++)
	{
		if( (bufferNOME[i] >= '0' && bufferNOME[i] <= '9') || (bufferNOME[i] >= 'A' && bufferNOME[i] <= 'Z') || (bufferNOME[i] >= 'a' && bufferNOME[i] <= 'z')) size++;
		else break;
	}
	
	uint8_t cont = 0;
	while(size >= 0)
	{
		if(bufferNOME[cont] == 'a' || bufferNOME[cont] == 'A') letraA(BLUE);
		else if(bufferNOME[cont] == 'b' || bufferNOME[cont] == 'B') letraB(BLUE);
		else if(bufferNOME[cont] == 'c' || bufferNOME[cont] == 'C') letraC(BLUE);
		else if(bufferNOME[cont] == 'd' || bufferNOME[cont] == 'D') letraD(GREEN);
		else if(bufferNOME[cont] == 'e' || bufferNOME[cont] == 'E') letraE(BLUE);
		else if(bufferNOME[cont] == 'f' || bufferNOME[cont] == 'F') letraF(BLUE);
		else if(bufferNOME[cont] == 'g' || bufferNOME[cont] == 'G') letraG(BLUE);
		else if(bufferNOME[cont] == 'h' || bufferNOME[cont] == 'H') letraH(BLUE);
		else if(bufferNOME[cont] == 'i' || bufferNOME[cont] == 'I') letraI(GREEN);
		else if(bufferNOME[cont] == 'j' || bufferNOME[cont] == 'J') letraJ(BLUE);
		else if(bufferNOME[cont] == 'k' || bufferNOME[cont] == 'K') letraK(BLUE);
		else if(bufferNOME[cont] == 'l' || bufferNOME[cont] == 'L') letraL(BLUE);
		else if(bufferNOME[cont] == 'm' || bufferNOME[cont] == 'M') letraM(BLUE);
		else if(bufferNOME[cont] == 'n' || bufferNOME[cont] == 'N') letraN(GREEN);
		else if(bufferNOME[cont] == 'o' || bufferNOME[cont] == 'O') letraO(GREEN);
		else if(bufferNOME[cont] == 'p' || bufferNOME[cont] == 'P') letraP(BLUE);
		else if(bufferNOME[cont] == 'q' || bufferNOME[cont] == 'Q') letraQ(BLUE);
		else if(bufferNOME[cont] == 'r' || bufferNOME[cont] == 'R') letraR(BLUE);
		else if(bufferNOME[cont] == 's' || bufferNOME[cont] == 'S') letraS(BLUE);
		else if(bufferNOME[cont] == 't' || bufferNOME[cont] == 'T') letraT(BLUE);
		else if(bufferNOME[cont] == 'u' || bufferNOME[cont] == 'U') letraU(BLUE);
		else if(bufferNOME[cont] == 'v' || bufferNOME[cont] == 'V') letraV(GREEN);
		else if(bufferNOME[cont] == 'w' || bufferNOME[cont] == 'W') letraW(BLUE);
		else if(bufferNOME[cont] == 'x' || bufferNOME[cont] == 'X') letraX(BLUE);
		else if(bufferNOME[cont] == 'y' || bufferNOME[cont] == 'Y') letraY(BLUE);
		else if(bufferNOME[cont] == 'z' || bufferNOME[cont] == 'Z') letraZ(BLUE);
		size--;
		cont++;
	}
	data_reset();
	delay_s(1);
	
	
	}
	return 0;
}

















//TESTEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEE
/*
while(1){
	
	//colorTESTE();
	//delay_ms(250);
	data_reset();
	
	for(int i = 0; i<1 ; i++){
		colorRED();
		colorGREEN();
		colorBLUE();
	}
	
	for(int i = 0; i<250 ; i++){
		colorBLACK();
	}
	for(int i = 0; i<2; i++){
		colorWHITE();
	}
	
	
	data_reset();
	
	delay_ms(250);
	
	for(int i = 0; i<1 ; i++){
		colorBLACK();
		colorBLACK();
		colorBLACK();
	}
	
	for(int i = 0; i<250 ; i++){
		colorBLACK();
	}
	for(int i = 0; i<2; i++){
		colorBLACK();
	}
	
	//for(int i = 0; i<290 ; i++){
	//	colorBLACK();
	//}
	
	data_reset();
	
	delay_ms(250);
	
}*/