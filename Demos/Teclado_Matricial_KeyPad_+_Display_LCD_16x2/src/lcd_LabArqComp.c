/*
 * Data: 19/03/2019 
 * 5º Semestre - Engenharia da Computação - Insper
 * Marco Mello - marcoasma@insper.edu.br
 */

#include "lcd_LabArqComp.h"

void Lcd4_Port(char a)
{
	if(a & 1)
	pio_set(D4_PIO, D4_PIO_IDX_MASK);
	else
	pio_clear(D4_PIO, D4_PIO_IDX_MASK);
	
	if(a & 2)
	pio_set(D5_PIO, D5_PIO_IDX_MASK);
	else
	pio_clear(D5_PIO, D5_PIO_IDX_MASK);
	
	if(a & 4)
	pio_set(D6_PIO, D6_PIO_IDX_MASK);
	else
	pio_clear(D6_PIO, D6_PIO_IDX_MASK);
	
	if(a & 8)
	pio_set(D7_PIO, D7_PIO_IDX_MASK);
	else
	pio_clear(D7_PIO, D7_PIO_IDX_MASK);
}

void Lcd4_Cmd(char a)
{
	pio_clear(RS_PIO, RS_PIO_IDX_MASK);            // => RS = 0
	Lcd4_Port(a);
	pio_set(EN_PIO, EN_PIO_IDX_MASK);             // => E = 1
	delay_ms(1);
	pio_clear(EN_PIO, EN_PIO_IDX_MASK);             // => E = 0
	delay_ms(1);
}

void Lcd4_Init()
{
	pmc_enable_periph_clk(D4_PIO_ID);
	pio_set_output(D4_PIO, D4_PIO_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(D5_PIO_ID);
	pio_set_output(D5_PIO, D5_PIO_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(D6_PIO_ID);
	pio_set_output(D6_PIO, D6_PIO_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(D7_PIO_ID);
	pio_set_output(D7_PIO, D7_PIO_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(RS_PIO_ID);
	pio_set_output(RS_PIO, RS_PIO_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(EN_PIO_ID);
	pio_set_output(EN_PIO, EN_PIO_IDX_MASK, 1, 0, 0);
	
	Lcd4_Port(0x00);
	delay_ms(20);
	///////////// Processo de Reset /////////
	Lcd4_Cmd(0x03);
	delay_ms(5);
	Lcd4_Cmd(0x03);
	delay_ms(11);
	Lcd4_Cmd(0x03);
	/////////////////////////////////////////
	Lcd4_Cmd(0x02);
	Lcd4_Cmd(0x02);
	Lcd4_Cmd(0x08);
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x0C);
	Lcd4_Cmd(0x00);
	Lcd4_Cmd(0x06);
}

void Lcd4_Clear()
{
	Lcd4_Cmd(0);
	Lcd4_Cmd(1);
}

void Lcd4_Set_Cursor(char a, char b)
{
	char temp,z,y;
	if(a == 0)
	{
		temp = 0x80 + b;
		z = temp>>4;
		y = (0x80+b) & 0x0F;
		Lcd4_Cmd(z);
		Lcd4_Cmd(y);
	}
	else if(a == 1)
	{
		temp = 0xC0 + b;
		z = temp>>4;
		y = (0xC0+b) & 0x0F;
		Lcd4_Cmd(z);
		Lcd4_Cmd(y);
	}
}

void Lcd4_Write_Char(char a)
{
	char temp,y;
	temp = a&0x0F;
	y = a&0xF0;
	pio_set(RS_PIO, RS_PIO_IDX_MASK);
	Lcd4_Port(y>>4);
	pio_set(EN_PIO, EN_PIO_IDX_MASK);
	delay_ms(1);
	pio_clear(EN_PIO, EN_PIO_IDX_MASK);
	delay_ms(1);
	Lcd4_Port(temp);
	pio_set(EN_PIO, EN_PIO_IDX_MASK);
	delay_ms(1);
	pio_clear(EN_PIO, EN_PIO_IDX_MASK);
	delay_ms(1);
}

void Lcd4_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	Lcd4_Write_Char(a[i]);
}

void Lcd4_Shift_Right()
{
	Lcd4_Cmd(0x01);
	Lcd4_Cmd(0x0C);
}

void Lcd4_Shift_Left()
{
	Lcd4_Cmd(0x01);
	Lcd4_Cmd(0x08);
}