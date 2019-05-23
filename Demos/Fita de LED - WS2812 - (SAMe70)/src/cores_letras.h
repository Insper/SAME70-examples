/*
 * cores.h
 *
 * Created: 21/05/2019 09:50:02
 *  Author: MarcoASMA
 */ 


#ifndef CORES_H_
#define CORES_H_



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
	for(int i=0;i<256;i++)
	{
		colorBLACK();
	}
	data_reset();
}




void letraA(int8_t red, int8_t green, int8_t blue)
{
	//A
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}




void letraB(int8_t red, int8_t green, int8_t blue)
{
	//B
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
		
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}


void letraC(int8_t red, int8_t green, int8_t blue)
{
	//C
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}



void letraD(int8_t red, int8_t green, int8_t blue)
{
	//D
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}


void letraE(int8_t red, int8_t green, int8_t blue)
{
	//E
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}



void letraF(int8_t red, int8_t green, int8_t blue)
{
	//F
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}






void letraG(int8_t red, int8_t green, int8_t blue)
{
	//G
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}


void letraH(int8_t red, int8_t green, int8_t blue)
{
	//H
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}


void letraI(int8_t red, int8_t green, int8_t blue)
{
	//I
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}



void letraJ(int8_t red, int8_t green, int8_t blue)
{
	//J
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}



void letraK(int8_t red, int8_t green, int8_t blue)
{
	//K
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}


void letraL(int8_t red, int8_t green, int8_t blue)
{
	//L
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}

void letraM(int8_t red, int8_t green, int8_t blue){
	//M
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();

	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	
}


void letraN(int8_t red, int8_t green, int8_t blue){
	//N
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();

	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
}


void letraO(int8_t red, int8_t green, int8_t blue){
	//O
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();

	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
}	
	

void letraP(int8_t red, int8_t green, int8_t blue){
	//P
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();

	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
}


void letraQ(int8_t red, int8_t green, int8_t blue){
	//Q
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();

	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}
	
	
void letraR(int8_t red, int8_t green, int8_t blue){
	//R
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();

	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}	
	
	
	
void letraS(int8_t red, int8_t green, int8_t blue){
	//S
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();

	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}
	
	
	
void letraT(int8_t red, int8_t green, int8_t blue){
	//T
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();;
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();

	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}
	
	
	
void letraU(int8_t red, int8_t green, int8_t blue){
		//U
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		
		
		colorBLACK();
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorBLACK();
		
		colorBLACK();
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		
		
		colorDEFINE(red,green,blue);
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		
		
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorDEFINE(red,green,blue);
		
		
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorDEFINE(red,green,blue);
		colorBLACK();
				

	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);

		
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
		colorBLACK();
	}
	
	
void letraV(int8_t red, int8_t green, int8_t blue){
	//V
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	

	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();

	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}
	
void letraW(int8_t red, int8_t green, int8_t blue){
	//W
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
}
	


void letraX(int8_t red, int8_t green, int8_t blue){
	//X
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	

	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
}



void letraY(int8_t red, int8_t green, int8_t blue){
	//Y
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	

	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}



void letraZ(int8_t red, int8_t green, int8_t blue){
	//Z
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	
	
	
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	
	
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	

	colorBLACK();
	colorDEFINE(red,green,blue);
	colorDEFINE(red,green,blue);
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorDEFINE(red,green,blue);
	
	
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
	colorBLACK();
}



void sequenciaA_Z()
{
	letraA(255,0,0);
	data_reset();
	delay_ms(250);
	letraB(255,0,0);
	data_reset();
	delay_ms(250);
	letraC(255,0,0);
	data_reset();
	delay_ms(250);
	letraD(255,0,0);
	data_reset();
	delay_ms(250);
	letraE(255,0,0);
	data_reset();
	delay_ms(250);
	letraF(255,0,0);
	data_reset();
	delay_ms(250);
	letraG(255,0,0);
	data_reset();
	delay_ms(250);
	letraH(255,0,0);
	data_reset();
	delay_ms(250);
	letraI(255,0,0);
	data_reset();
	delay_ms(250);
	
	
	
	
	
	letraJ(255,0,0);
	data_reset();
	delay_ms(250);
	letraK(255,0,0);
	data_reset();
	delay_ms(250);
	letraL(255,0,0);
	data_reset();
	delay_ms(250);
	letraM(255,0,0);
	data_reset();
	delay_ms(250);
	letraN(255,0,0);
	data_reset();
	delay_ms(250);
	letraO(255,0,0);
	data_reset();
	delay_ms(250);
	letraP(255,0,0);
	data_reset();
	delay_ms(250);
	letraQ(255,0,0);
	data_reset();
	delay_ms(250);
	letraR(255,0,0);
	data_reset();
	delay_ms(250);
	letraS(255,0,0);
	data_reset();
	delay_ms(250);
	letraT(255,0,0);
	data_reset();
	delay_ms(250);
	letraU(255,0,0);
	data_reset();
	delay_ms(250);
	letraV(255,0,0);
	data_reset();
	delay_ms(250);
	letraX(255,0,0);
	data_reset();
	delay_ms(250);
	letraY(255,0,0);
	data_reset();
	delay_ms(250);
	letraZ(255,0,0);
	data_reset();
	delay_ms(250);
}

#endif /* CORES_H_ */