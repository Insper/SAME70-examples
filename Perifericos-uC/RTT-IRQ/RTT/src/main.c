/**
 * 5 semestre - Eng. da Computação - Insper
 * Rafael Corsi - rafael.corsi@insper.edu.br
 *
 * Projeto 0 para a placa SAME70-XPLD
 *
 *
 * Material :
 *  - Kit: ATMEL SAME70-XPLD - ARM CORTEX M7
 */

/************************************************************************/
/* includes                                                             */
/************************************************************************/

#include "asf.h"

/************************************************************************/
/* defines                                                              */
/************************************************************************/

#define LED_PIO       PIOC
#define LED_PIO_ID    ID_PIOC
#define LED_IDX       8u
#define LED_IDX_MASK  (1u << LED_IDX)

/************************************************************************/
/* constants                                                            */
/************************************************************************/

/************************************************************************/
/* variaveis globais                                                    */
/************************************************************************/
volatile int flag_rtt;

/************************************************************************/
/* prototypes                                                           */
/************************************************************************/
void pin_toggle(Pio *pio, uint32_t mask);
void io_init(void);
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);

/************************************************************************/
/* interrupcoes                                                         */
/************************************************************************/

void RTT_Handler(void) {
  uint32_t ul_status;
  ul_status = rtt_get_status(RTT);

  /* IRQ due to Alarm */
  if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
      flag_rtt = 1;
   }  
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void pin_toggle(Pio *pio, uint32_t mask){
  if(pio_get_output_data_status(pio, mask))
    pio_clear(pio, mask);
  else
    pio_set(pio,mask);
}

void io_init(void){
  pmc_enable_periph_clk(LED_PIO_ID);
  pio_configure(LED_PIO, PIO_OUTPUT_1, LED_IDX_MASK, PIO_DEFAULT);
}

static float get_time_rtt(){
  uint ul_previous_time = rtt_read_timer_value(RTT); 
}

/** 
 * Configura RTT
 *
 * arg0 pllPreScale  : Frequência na qual o contador irá incrementar
 * arg1 IrqNPulses   : Valor do alarme 
 * arg2 rttIRQSource : Pode ser uma 
 *     - 0: 
 *     - RTT_MR_RTTINCIEN: Interrupção por incremento (pllPreScale)
 *     - RTT_MR_ALMIEN : Interrupção por alarme
 */
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

  uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
	
  rtt_sel_source(RTT, false);
  rtt_init(RTT, pllPreScale);
  
  if (rttIRQSource & RTT_MR_ALMIEN) {
	uint32_t ul_previous_time;
  	ul_previous_time = rtt_read_timer_value(RTT);
  	while (ul_previous_time == rtt_read_timer_value(RTT));
  	rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);
  }

  /* config NVIC */
  NVIC_DisableIRQ(RTT_IRQn);
  NVIC_ClearPendingIRQ(RTT_IRQn);
  NVIC_SetPriority(RTT_IRQn, 4);
  NVIC_EnableIRQ(RTT_IRQn);

  /* Enable RTT interrupt */
  if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
	rtt_enable_interrupt(RTT, rttIRQSource);
  else
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
		  
}

/************************************************************************/
/* Main                                                                 */
/************************************************************************/

// Funcao principal chamada na inicalizacao do uC.
int main(void){
  sysclk_init();
  WDT->WDT_MR = WDT_MR_WDDIS;
  io_init();
    	
  flag_rtt = 1; // força flag = 1 para inicializar alarme.
  while (1) {
   if(flag_rtt) {
	  pin_toggle(LED_PIO, LED_IDX_MASK);    // BLINK Led
	  RTT_init(4, 16, RTT_MR_ALMIEN);  // inicializa rtt com alarme
	  flag_rtt = 0;
    }   
  }  
}
