#include "conf_board.h"
#include <asf.h>

/************************************************************************/
/* defines                                                              */
/************************************************************************/

// TRIGGER
#define TRIGGER_PIO PIOC
#define TRIGGER_PIO_ID ID_PIOC
#define TRIGGER_IDX 8
#define TRIGGER_IDX_MASK (1 << TRIGGER_IDX)

// Botão
#define ECHO_PIO PIOD
#define ECHO_PIO_ID ID_PIOD
#define ECHO_IDX 20
#define ECHO_IDX_MASK (1 << ECHO_IDX)

// Timer
#define TIMER_CHANNEL 1
#define TIMER_ID ID_TC1
#define TIMER TC0
#define TIMER_FREQ 42

/** prototypes */
void but_callback(void);
static void ECHO_init(void);
static void USART1_init(void);
uint32_t usart_puts(uint8_t *pstring);

/* globals */
uint32_t g_tcValue = 0;
uint32_t g_done = 0;

/************************************************************************/
/* handlers / callbacks                                                 */
/************************************************************************/

void signalCallback(void) {
  if (!pio_get(ECHO_PIO, PIO_INPUT, ECHO_IDX_MASK)) {
    g_tcValue = 0;
  } else {
    g_done = 1;
  }
}

void TC1_Handler(void) {
  volatile uint32_t status = tc_get_status(TC0, 1);
  g_tcValue++;
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void TC_init(Tc *TC, int ID_TC, int TC_CHANNEL, int freq) {
  uint32_t ul_div;
  uint32_t ul_tcclks;
  uint32_t ul_sysclk = sysclk_get_cpu_hz();

  /* Configura o PMC */
  pmc_enable_periph_clk(ID_TC);

  /** Configura o TC para operar em  freq hz e interrupçcão no RC compare */
  tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
  tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
  tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

  /* Configura NVIC*/
  NVIC_SetPriority(ID_TC, 4);
  NVIC_EnableIRQ((IRQn_Type)ID_TC);
  tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);
}

void io_init(void) {

  // Configura led
  pmc_enable_periph_clk(TRIGGER_PIO_ID);
  pio_configure(TRIGGER_PIO, PIO_OUTPUT_0, TRIGGER_IDX_MASK, PIO_DEFAULT);

  // Inicializa clock do periférico PIO responsavel pelo botao
  pmc_enable_periph_clk(ECHO_PIO_ID);

  // Configura PIO para lidar com o pino do botão como entrada
  // com pull-up
  pio_configure(ECHO_PIO, PIO_INPUT, ECHO_IDX_MASK, PIO_PULLUP);

  // Configura interrupção no pino referente ao botao e associa
  // função de callback caso uma interrupção for gerada
  // a função de callback é a: but_callback()
  pio_handler_set(ECHO_PIO, ECHO_PIO_ID, ECHO_IDX_MASK, PIO_IT_EDGE,
                  signalCallback);

  // Ativa interrupção
  pio_enable_interrupt(ECHO_PIO, ECHO_IDX_MASK);

  // Configura NVIC para receber interrupcoes do PIO do botao
  // com prioridade 4 (quanto mais próximo de 0 maior)
  NVIC_EnableIRQ(ECHO_PIO_ID);
  NVIC_SetPriority(ECHO_PIO_ID, 4); // Prioridade 4
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/

void main(void) {

  /* Initialize the SAM system */
  sysclk_init();
  board_init();

  /* Initialize the console uart */
  configure_console();

  /* Start timer */
  TC_init(TC0, ID_TC1, 1, 32000);
  tc_start(TC0, 1);

  while (1) {

    pio_set(TRIGGER_PIO, TRIGGER_IDX_MASK);
    delay_us(10);
    pio_clear(TRIGGER_PIO, TRIGGER_IDX_MASK);
    printf("Trigger \n");

    if (g_done) {
      /* D = t(s) * Vel Sound (m/s) / 2 */
      float ts = (float)g_tcValue / 32000.0;
      float dm = ts * 340 / 2;
      g_done = 0;

      printf("%d\n", (int)(dm * 100));
    }

    delay_ms(500);
  }
}
