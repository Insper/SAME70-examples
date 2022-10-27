#include "ledColors.h"

void colorTESTE(void)
{
	for(uint8_t cor=0; cor<2; cor++) data_0();
}

void colorRED(void)
{
	for(uint8_t cor=0; cor<8; cor++) data_0();
	for(uint8_t cor=0; cor<8; cor++) data_1();
	for(uint8_t cor=0; cor<8; cor++) data_0();
}

void colorGREEN(void)
{
	for(uint8_t cor=0; cor<8; cor++) data_1();
	for(uint8_t cor=0; cor<8; cor++) data_0();
	for(uint8_t cor=0; cor<8; cor++) data_0();
}

void colorBLUE(void)
{
	for(uint8_t cor=0; cor<8; cor++) data_0();
	for(uint8_t cor=0; cor<8; cor++) data_0();
	for(uint8_t cor=0; cor<8; cor++) data_1();
}

void colorWHITE(void)
{
	for(uint8_t cor=0; cor<8; cor++) data_1();
	for(uint8_t cor=0; cor<8; cor++) data_1();
	for(uint8_t cor=0; cor<8; cor++) data_1();
}

void colorBLACK(void)
{
	for(uint8_t cor=0; cor<8; cor++) data_0();
	for(uint8_t cor=0; cor<8; cor++) data_0();
	for(uint8_t cor=0; cor<8; cor++) data_0();
}

void colorDEFINE(int8_t red, int8_t green, int8_t blue)
{
	int8_t count;
	int8_t bit;
	
	for (count = 7; count >= 0; count--)
	{
		bit = green >> count;
		if (bit & 1) data_1();
		else data_0();
	}
	for (count = 7; count >= 0; count--)
	{
		bit = red >> count;
		if (bit & 1) data_1();
		else data_0();
	}
	for (count = 7; count >= 0; count--)
	{
		bit = blue >> count;
		if (bit & 1) data_1();
		else data_0();
	}
}


void setOneLED(uint32_t posicao, uint8_t estado)
{
	posicao=posicao-1;
	if(estado == 1)
	{
		for(int i=0;i<posicao;i++)
		{
			colorBLACK();
		}
		colorRED();
	}
	
	if(estado == 0)
	{
		for(int i=0;i<posicao;i++)
		{
			colorBLACK();
		}
		colorBLACK();
	}
	
	
	data_reset();
	return 0;
}



void clearLEDs(void)
{
	for(int i=0;i<LEDS_NUMBER;i++)
	{
		colorBLACK();
	}
	data_reset();
}