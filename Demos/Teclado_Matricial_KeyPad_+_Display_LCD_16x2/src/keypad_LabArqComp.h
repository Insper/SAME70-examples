
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


void keypad_init()
{
		
	pmc_enable_periph_clk(KEY_LIN_1_ID);
	pio_set_output(KEY_LIN_1, KEY_LIN_1_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(KEY_LIN_2_ID);
	pio_set_output(KEY_LIN_2, KEY_LIN_2_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(KEY_LIN_3_ID);
	pio_set_output(KEY_LIN_3, KEY_LIN_3_IDX_MASK, 1, 0, 0);
	
	pmc_enable_periph_clk(KEY_LIN_4_ID);
	pio_set_output(KEY_LIN_4, KEY_LIN_4_IDX_MASK, 1, 0, 0);
	
	
	
	pmc_enable_periph_clk(KEY_COL_1_ID);
	pio_set_input(KEY_COL_1, KEY_COL_1_IDX_MASK, PIO_PULLUP);
	
	pmc_enable_periph_clk(KEY_COL_2_ID);
	pio_set_input(KEY_COL_2, KEY_COL_2_IDX_MASK, PIO_PULLUP);
	
	pmc_enable_periph_clk(KEY_COL_3_ID);
	pio_set_input(KEY_COL_3, KEY_COL_3_IDX_MASK, PIO_PULLUP);
	
	pmc_enable_periph_clk(KEY_COL_4_ID);
	pio_set_input(KEY_COL_4, KEY_COL_4_IDX_MASK, PIO_PULLUP);/**/
}

char le_keypad()
{
	char lido = '\0';
	
	pio_clear(KEY_LIN_1, KEY_LIN_1_IDX_MASK);
	pio_set(KEY_LIN_2, KEY_LIN_2_IDX_MASK);
	pio_set(KEY_LIN_3, KEY_LIN_3_IDX_MASK);
	pio_set(KEY_LIN_4, KEY_LIN_4_IDX_MASK);

	if(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
		{
		delay_ms(20);
		}
		lido = '1';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '2';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '3';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = 'A';
		delay_ms(20);
	}


	pio_set(KEY_LIN_1, KEY_LIN_1_IDX_MASK);
	pio_clear(KEY_LIN_2, KEY_LIN_2_IDX_MASK);
	pio_set(KEY_LIN_3, KEY_LIN_3_IDX_MASK);
	pio_set(KEY_LIN_4, KEY_LIN_4_IDX_MASK);

	if(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
		{
		delay_ms(20);	
		}
		lido = '4';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '5';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '6';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = 'B';
		delay_ms(20);
	}


	pio_set(KEY_LIN_1, KEY_LIN_1_IDX_MASK);
	pio_set(KEY_LIN_2, KEY_LIN_2_IDX_MASK);
	pio_clear(KEY_LIN_3, KEY_LIN_3_IDX_MASK);
	pio_set(KEY_LIN_4, KEY_LIN_4_IDX_MASK);

	if(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '7';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '8';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '9';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = 'C';
		delay_ms(20);
	}


	pio_set(KEY_LIN_1, KEY_LIN_1_IDX_MASK);
	pio_set(KEY_LIN_2, KEY_LIN_2_IDX_MASK);
	pio_set(KEY_LIN_3, KEY_LIN_3_IDX_MASK);
	pio_clear(KEY_LIN_4, KEY_LIN_4_IDX_MASK);

	if(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_1,PIO_INPUT,KEY_COL_1_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '*';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_2,PIO_INPUT,KEY_COL_2_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '0';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_3,PIO_INPUT,KEY_COL_3_IDX_MASK)))
		{
			delay_ms(20);
		}
		lido = '#';
		delay_ms(20);
	}

	else if(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
	{
		while(!(pio_get(KEY_COL_4,PIO_INPUT,KEY_COL_4_IDX_MASK)))
		{
		delay_ms(20);
		}
		lido = 'D';
		delay_ms(20);
	}

	return lido;

}