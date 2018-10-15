/************************************************************************
* 5 semestre - Eng. da Computao - Insper
* Rafael Corsi - rafael.corsi@insper.edu.br
*
* Material:
*  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
*
* Objetivo:
*  - Demonstrar do uso do TC e RTC
*
* Periféricos:
*  - TC
*  - RTC
*  - PIO
*  - PMC
*
* Log:
*  - 10/2018: Criação
*************************************************************************/

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/

/**
 *  Informacoes para o RTC
 *  poderia ser extraida do __DATE__ e __TIME__
 *  ou ser atualizado pelo PC.
 */
#define YEAR        2018
#define MOUNTH      3
#define DAY         19
#define WEEK        18
#define HOUR        15
#define MINUTE      45
#define SECOND      0

// LEDs
#define LED_PIO_ID		 ID_PIOC
#define LED_PIO        PIOC
#define LED_IDX		     8
#define LED_IDX_MASK   (1<<LED_IDX)

// Botão
#define BUT_PIO_ID       ID_PIOA
#define BUT_PIO          PIOA
#define BUT_IDX		       11
#define BUT_IDX_MASK     (1 << BUT_IDX)
#define BUT_DEBOUNCING_VALUE  79

/************************************************************************/
/* globais                                                              */
/************************************************************************/

// toda variável global que for alterada dentro de uma interrupcao
// deve possuir o pragma volatile.
volatile uint8_t flag_led0 = 1;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/

void BUT_init(void);
void LED_init(int estado);
void TC_init(int freq, int TC, int ID_TC, int TC_CHANNEL);
void RTC_init(void);
void pin_toggle(Pio *pio, uint32_t mask);

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

/**
 *  Interrupt handler for TC1 interrupt.
 */
void TC1_Handler(void)
{
	volatile uint32_t ul_dummy;

  /****************************************************************
	* Devemos indicar ao TC que a interrupção foi satisfeita.
  ******************************************************************/
	ul_dummy = tc_get_status(TC0, 1);

	/* Avoid compiler warning */
	UNUSED(ul_dummy);

	/** Muda o estado do LED */
  if(flag_led0)
    pin_toggle(LED_PIO, LED_IDX_MASK);
}

/**
 * \brief Interrupt handler for the RTC. Refresh the display.
 */
void RTC_Handler(void)
{
  // Limpa interrupção
	uint32_t ul_status = rtc_get_status(RTC);

  // Podemos entrar na interrupção do RTC por vários
  // precisamos verificar qual a razão de termos entrado
  // aqui. Por isso dos ifs.

  // Second increment interrupt
	if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC)
  {
    /* limpa interrupcao */
		rtc_clear_status(RTC, RTC_SCCR_SECCLR);
	}
  else
  {
		/* Time or date alarm */
		if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM)
    {
      /* para o piscar do led */
      flag_led0 = 0;

      /* limpa interrupcao */
			rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
		}
	}
}


/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

/**
 *  Toggle pin controlado pelo PIO (out)
 */
void pin_toggle(Pio *pio, uint32_t mask)
{
   if(pio_get_output_data_status(pio, mask))
    pio_clear(pio, mask);
   else
    pio_set(pio,mask);
}

/**
 * @Brief Inicializa o pino do LED
 */
void LED_init(int estado)
{
    pmc_enable_periph_clk(LED_PIO_ID);
    pio_set_output(LED_PIO, LED_IDX_MASK, estado, 0, 0 );
};

/**
 * Configura TimerCounter (TC0) para gerar uma interrupcao no canal 0-(ID_TC1)
 * a cada 250 ms (4Hz)
 */
void TC_init(int freq, int TC, int ID_TC, int TC_CHANNEL)
{
    uint32_t ul_div;
    uint32_t ul_tcclks;
    uint32_t ul_sysclk = sysclk_get_cpu_hz();

    uint32_t channel = 1;

    /* Configura o PMC */
    /* O TimerCounter é meio confuso
       o uC possui 3 TCs, cada TC possui 3 canais
       TC0 : ID_TC0, ID_TC1, ID_TC2
       TC1 : ID_TC3, ID_TC4, ID_TC5
       TC2 : ID_TC6, ID_TC7, ID_TC8
    */
    pmc_enable_periph_clk(ID_TC);

    /** Configura o TC para operar em  4Mhz e interrupçcão no RC compare */
    tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
    tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
    tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

    /* Configura e ativa interrupçcão no TC canal 0 */
    /* Interrupção no C */
    NVIC_EnableIRQ((IRQn_Type) ID_TC);
    tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

    /* Inicializa o canal 0 do TC */
    tc_start(TC, TC_CHANNEL);
}

/**
 * Configura o RTC para funcionar com interrupcao de alarme
 */
void RTC_init()
{
    /* Configura o PMC */
    pmc_enable_periph_clk(ID_RTC);

    /* Default RTC configuration, 24-hour mode */
    rtc_set_hour_mode(RTC, 0);

    /* Configura data e hora manualmente */
    rtc_set_date(RTC, YEAR, MOUNTH, DAY, WEEK);
    rtc_set_time(RTC, HOUR, MINUTE, SECOND);

    /* Configure RTC interrupts */
    NVIC_DisableIRQ(RTC_IRQn);
    NVIC_ClearPendingIRQ(RTC_IRQn);
    NVIC_SetPriority(RTC_IRQn, 0);
    NVIC_EnableIRQ(RTC_IRQn);

    /* Ativa interrupcao via alarme */
    rtc_enable_interrupt(RTC,  RTC_IER_ALREN);
}

/************************************************************************/
/* Main Code	                                                        */
/************************************************************************/
int main(void)
{
  	// Inicializa clock
  	sysclk_init();

  	/* Disable the watchdog */
  	WDT->WDT_MR = WDT_MR_WDDIS;

    /* inicializa variaveis globais */
    flag_led0 = 0;

    /* Configura Leds */
    LED_init(0);

    /* Configura timer TC0 - canal 1 */
    TC_init(4, TC0, ID_TC1, 1);

    /* Configura RTC */
    RTC_init();

    /* configura alarme do RTC
     *  para um 30 segundos após o inicio do sistema */
    rtc_set_date_alarm(RTC, 1, MOUNTH, 1, DAY);
    rtc_set_time_alarm(RTC, 1, HOUR,   1, MINUTE+1, 1, SECOND);

  	while (1)
    {
      /* Entrar em modo sleep */
  	}
}
