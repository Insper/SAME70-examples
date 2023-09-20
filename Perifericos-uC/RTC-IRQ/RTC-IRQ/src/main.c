#include "asf.h"

/************************************************************************/
/* DEFINES                                                              */
/************************************************************************/

/**
 *  Informacoes para o RTC
 *  poderia ser extraida do __DATE__ e __TIME__
 *  ou ser atualizado pelo PC.
 */
typedef struct  {
  uint32_t year;
  uint32_t month;
  uint32_t day;
  uint32_t week;
  uint32_t hour;
  uint32_t minute;
  uint32_t second;
} calendar;

/* LEDs */
#define LED_PIO         PIOC
#define LED_PIO_ID      ID_PIOC
#define LED_PIO_IDX	8
#define LED_IDX_MASK    (1<<LED_PIO_IDX)

// LED 1
#define LED1_PIO PIOA
#define LED1_PIO_ID ID_PIOA
#define LED1_PIO_IDX 0
#define LED1_PIO_IDX_MASK (1 << LED1_PIO_IDX)

/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/
volatile char flag_rtc_alarm = 0;
volatile char flag_rtc_alarm1 = 0;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void LED_init(int estado);
void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type);
void pisca_led(int n, int t, int l);

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

/**
* \brief Interrupt handler for the RTC. Refresh the display.
*/
void RTC_Handler(void) {
    uint32_t ul_status = rtc_get_status(RTC);
	
    /* seccond tick */
    if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {	
			// o código para irq de segundo vem aqui
			printf("Sec alarm \n");
			flag_rtc_alarm1 = 1;
    }
	
    /* Time or date alarm */
    if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
    	// o código para irq de alame vem aqui
        flag_rtc_alarm = 1;
				printf("Time alarm \n");
    }

    rtc_clear_status(RTC, RTC_SCCR_SECCLR);
    rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
    rtc_clear_status(RTC, RTC_SCCR_ACKCLR);
    rtc_clear_status(RTC, RTC_SCCR_TIMCLR);
    rtc_clear_status(RTC, RTC_SCCR_CALCLR);
    rtc_clear_status(RTC, RTC_SCCR_TDERRCLR);
}


/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

void pisca_led (int n, int t, int l) {
	if (l) {
		for (int i=0;i<n;i++){
			pio_clear(LED1_PIO, LED1_PIO_IDX_MASK);
			delay_ms(t);
			pio_set(LED1_PIO, LED1_PIO_IDX_MASK);
			delay_ms(t);
		}
	} else {
		for (int i=0;i<n;i++){
			pio_clear(LED_PIO, LED_IDX_MASK);
			delay_ms(t);
			pio_set(LED_PIO, LED_IDX_MASK);
			delay_ms(t);
		}
	}
}

/**
* @Brief Inicializa o pino do LED
*/
void LED_init(int estado) {
     pmc_enable_periph_clk(LED_PIO_ID);
     pio_set_output(LED_PIO, LED_IDX_MASK, estado, 0, 0 );
		 
		 pmc_enable_periph_clk(LED1_PIO_ID);
		 pio_set_output(LED1_PIO, LED1_PIO_IDX_MASK, estado, 0, 0 );
};

/**
* Configura o RTC para funcionar com interrupcao de alarme
*/
void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type) {
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);

	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(rtc, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(rtc, t.year, t.month, t.day, t.week);
	rtc_set_time(rtc, t.hour, t.minute, t.second);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(id_rtc);
	NVIC_ClearPendingIRQ(id_rtc);
	NVIC_SetPriority(id_rtc, 4);
	NVIC_EnableIRQ(id_rtc);

	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(rtc,  irq_type);
}

static void configure_console(void) {
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	stdio_serial_init(CONF_UART, &uart_serial_options);

	/* Specify that stdout should not be buffered. */
	setbuf(stdout, NULL);
}

/************************************************************************/
/* Main Code	                                                        */
/************************************************************************/
int main(void) {
    /* Initialize the SAM system */                                                                 
    sysclk_init();        
		
		/* Initialize the console uart */
		configure_console();                                                                          
                                                                                                    
    /* Disable the watchdog */                                                                      
    WDT->WDT_MR = WDT_MR_WDDIS;                                                                     
                                                                                                    
    /* Configura Leds */                                                                            
    LED_init(1);                                                                                    
                                                                                                    
    /** Configura RTC */                                                                            
    calendar rtc_initial = {2018, 3, 19, 12, 15, 45 ,1};                                            
    RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_ALREN); 
		RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_SECEN);                                              
                                                                                                    
    /* Leitura do valor atual do RTC */           
    uint32_t current_hour, current_min, current_sec;
    uint32_t current_year, current_month, current_day, current_week;
    rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
    rtc_get_date(RTC, &current_year, &current_month, &current_day, &current_week);
	
    /* configura alarme do RTC para daqui 20 segundos */                                                                   
    rtc_set_date_alarm(RTC, 1, current_month, 1, current_day);                              
    rtc_set_time_alarm(RTC, 1, current_hour, 1, current_min, 1, current_sec + 10);
		printf("Chegou \n");
                                                                                                    
    while (1) {                                                                                     
      if(flag_rtc_alarm){                                                                                 
          pisca_led(5, 200, 0);                                                                       
          flag_rtc_alarm = 0;                                                                          
       }
			 if(flag_rtc_alarm1){
				 pisca_led(5, 200, 1);
				 flag_rtc_alarm1 = 0;
			 }                                                                                          
    }                                                                                               
}
