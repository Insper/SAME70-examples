#define KEY_LIN_1				PIOC
#define KEY_LIN_1_ID			ID_PIOC
#define KEY_LIN_1_IDX			31
#define KEY_LIN_1_IDX_MASK (1 << KEY_LIN_1_IDX)

#define KEY_LIN_2				PIOB
#define KEY_LIN_2_ID			ID_PIOB
#define KEY_LIN_2_IDX			3
#define KEY_LIN_2_IDX_MASK (1 << KEY_LIN_2_IDX)

#define KEY_LIN_3				PIOA
#define KEY_LIN_3_ID			ID_PIOA
#define KEY_LIN_3_IDX			0
#define KEY_LIN_3_IDX_MASK (1 << KEY_LIN_3_IDX)

#define KEY_LIN_4				PIOD
#define KEY_LIN_4_ID			ID_PIOD
#define KEY_LIN_4_IDX			28
#define KEY_LIN_4_IDX_MASK (1 << KEY_LIN_4_IDX)

#define KEY_COL_1				PIOC
#define KEY_COL_1_ID			ID_PIOC
#define KEY_COL_1_IDX			17
#define KEY_COL_1_IDX_MASK (1 << KEY_COL_1_IDX)

#define KEY_COL_2				PIOC
#define KEY_COL_2_ID			ID_PIOC
#define KEY_COL_2_IDX			30
#define KEY_COL_2_IDX_MASK (1 << KEY_COL_2_IDX)

#define KEY_COL_3				PIOB
#define KEY_COL_3_ID			ID_PIOB
#define KEY_COL_3_IDX			2
#define KEY_COL_3_IDX_MASK (1 << KEY_COL_3_IDX)

#define KEY_COL_4				PIOA
#define KEY_COL_4_ID			ID_PIOA
#define KEY_COL_4_IDX			19
#define KEY_COL_4_IDX_MASK (1 << KEY_COL_4_IDX)

void le_keypad()

{


pio_clear(KEY_LIN_1, KEY_LIN_1_IDX_MASK);
pio_set(KEY_LIN_2, KEY_LIN_2_IDX_MASK);
pio_set(KEY_LIN_3, KEY_LIN_3_IDX_MASK);
pio_set(KEY_LIN_4, KEY_LIN_4_IDX_MASK);

if(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('1');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('2');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('3');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('A');
	delay_ms(10);
}


pio_set(KEY_LIN_1, KEY_LIN_1_IDX_MASK);
pio_clear(KEY_LIN_2, KEY_LIN_2_IDX_MASK);
pio_set(KEY_LIN_3, KEY_LIN_3_IDX_MASK);
pio_set(KEY_LIN_4, KEY_LIN_4_IDX_MASK);

if(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('4');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('5');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('6');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('B');
	delay_ms(10);
}


pio_set(KEY_LIN_1, KEY_LIN_1_IDX_MASK);
pio_set(KEY_LIN_2, KEY_LIN_2_IDX_MASK);
pio_clear(KEY_LIN_3, KEY_LIN_3_IDX_MASK);
pio_set(KEY_LIN_4, KEY_LIN_4_IDX_MASK);

if(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('7');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('8');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('9');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('C');
	delay_ms(10);
}


pio_set(KEY_LIN_1, KEY_LIN_1_IDX_MASK);
pio_set(KEY_LIN_2, KEY_LIN_2_IDX_MASK);
pio_set(KEY_LIN_3, KEY_LIN_3_IDX_MASK);
pio_clear(KEY_LIN_4, KEY_LIN_4_IDX_MASK);

if(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('*');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('0');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('#');
	delay_ms(10);
}

else if(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
{
	Lcd4_Clear();
	Lcd4_Set_Cursor(1,0);
	Lcd4_Write_Char('D');
	delay_ms(10);
}


return 0;

	
}