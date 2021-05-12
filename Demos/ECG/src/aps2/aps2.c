/*
 * aps2.c
 *
 * Created: 10/05/2021 11:11:07
 *  Author: RafaelCF4
 */ 

#include "aps2.h"

void TC0_Handler(void) {
  volatile uint32_t ul_dummy;

  ul_dummy = tc_get_status(TC0, 0);

  if (g_ecgCnt >= g_ecgSize) {
    g_ecgCnt = 0;
    g_ecgDelayCnt = g_ecgDelayValue;
  }

  if (g_ecgDelayCnt > 0) {
    g_ecgDelayCnt--;
    } else {
    g_ecgCnt++;
  }
  dacc_write_conversion_data(DACC_BASE, ecg[g_ecgCnt], DACC_CHANNEL);
}

static void config_DAC(void) {
  /* Enable clock for DACC */
  sysclk_enable_peripheral_clock(DACC_ID);

  /* Reset DACC registers */
  dacc_reset(DACC_BASE);

  /* Half word transfer mode */
  dacc_set_transfer_mode(DACC_BASE, 0);
  dacc_set_osr(DACC_BASE, DACC_CHANNEL, 5 );

  /* Enable output channel DACC_CHANNEL */
  dacc_enable_channel(DACC_BASE, DACC_CHANNEL);

  /* Set up analog current */
  dacc_set_analog_control(DACC_BASE, DACC_ANALOG_CONTROL);
}

void TC_init_modificado(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
  uint32_t ul_div;
  uint32_t ul_tcclks;
  uint32_t ul_sysclk = sysclk_get_cpu_hz();

  pmc_enable_periph_clk(ID_TC);
  tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
  tc_init(TC, TC_CHANNEL, TC_CMR_TCCLKS_TIMER_CLOCK2 | TC_CMR_CPCTRG);
  tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / 16) / freq);

  NVIC_SetPriority(ID_TC, 4);
  NVIC_EnableIRQ((IRQn_Type) ID_TC);
  tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

  tc_start(TC, TC_CHANNEL);
}

void vTimerEcgCallback( TimerHandle_t xTimer ) {
  // delay entre ECGs de [200, 500]
  g_ecgDelayValue = rand() % (500 + 1 - 200) + 200;
 // printf("%d \n", g_ecgDelayValue);
}

void task_aps2(void *pvParameters) {
  
  g_ecgDelayValue = 200;
  config_DAC();
  TC_init_modificado(TC0, ID_TC0, 0, DAC_FS);
  xTimer = xTimerCreate("ECG", pdMS_TO_TICKS(ECG_CHANGE_TIME), pdTRUE, (void *) 0, vTimerEcgCallback);
  xTimerStart( xTimer, 0 );
  
  // oxiometria
  char ox;
  float theta = 0;

  for (;;)  {
    
    // Oxiometria: sin entre 88 e 100
    if (theta > 2*PI) {
      theta = 0;
    }
    ox = (char) ((arm_sin_f32(theta) + 1.0)*12.0/2.0 + 88.0);
    theta = theta + 0.3;
    xQueueSend(xQueueOx, &ox, 100);

    vTaskDelay(2000);
  }
}

