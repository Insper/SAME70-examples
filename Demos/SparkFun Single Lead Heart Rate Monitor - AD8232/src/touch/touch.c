/*
* touch.c
*
* Created: 10/02/2021 17:42:26
*  Author: RafaelCF4
*/

#include <asf.h>
#include "touch.h"
#include "conf_touch.h"

static void config_AFEC_touch(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback) {
  /*************************************
  * Ativa e configura AFEC
  *************************************/
  /* struct de configuracao do AFEC */
  struct afec_config afec_cfg;

  /* Carrega parametros padrao */
  afec_get_config_defaults(&afec_cfg);

  /* Configura AFEC */
  afec_init(afec, &afec_cfg);

  /* Configura trigger por software */
  afec_set_trigger(afec, AFEC_TRIG_FREERUN);

  /*** Configuracao específica do canal AFEC ***/
  struct afec_ch_config afec_ch_cfg;
  afec_ch_get_config_defaults(&afec_ch_cfg);
  afec_ch_cfg.gain = AFEC_GAINVALUE_0;
  afec_ch_set_config(afec, afec_channel, &afec_ch_cfg);

  /* Calibracao: */
  afec_channel_set_analog_offset(afec, afec_channel, 0x200);
  afec_channel_enable(afec, afec_channel);
}

void configure_touch(void) {
  /*
  * Xm : PIN4 - PC13 - AFEC1_AD1
  * Ym : PIN6 - PD11
  * Xp : PIN7 - PC19
  * Yp : PIN3 - PD30 - AFEC0_AD0
  */
  pmc_enable_periph_clk(XM_PIO_ID);
  pmc_enable_periph_clk(YP_PIO_ID);
  pmc_enable_periph_clk(XP_PIO_ID);
  pmc_enable_periph_clk(YM_PIO_ID);

  afec_enable(XM_AFEC);
  afec_enable(YP_AFEC);
  config_AFEC_touch(YP_AFEC, YP_AFEC_ID, YP_CHANNEL, NULL);
  config_AFEC_touch(XM_AFEC, XM_AFEC_ID, XM_CHANNEL, NULL);
}

int readYp(void) {
  return(afec_channel_get_value(YP_AFEC, YP_CHANNEL));
}

int readXm(void) {
  return(afec_channel_get_value(XM_AFEC, XM_CHANNEL));
}

void configReadTouchX(void){
  afec_channel_enable(YP_AFEC, YP_CHANNEL);
  afec_channel_disable(XM_AFEC, XM_CHANNEL);
  pio_configure(XP_PIO, PIO_OUTPUT_1, XP_MASK, PIO_DEFAULT);
  pio_configure(XM_PIO, PIO_OUTPUT_0, XM_MASK, PIO_DEFAULT);
  pio_configure(YM_PIO, PIO_INPUT, YM_MASK, PIO_DEFAULT);
  pio_configure(YP_PIO, PIO_INPUT, YP_MASK, PIO_DEFAULT);
  delay_ms(5);
}

void configReadTouchY(void) {
  afec_channel_disable(YP_AFEC, YP_CHANNEL);
  afec_channel_enable(XM_AFEC, XM_CHANNEL);
  pio_configure(YP_PIO, PIO_OUTPUT_1, YP_MASK, PIO_DEFAULT);
  pio_configure(YM_PIO, PIO_OUTPUT_0, YM_MASK, PIO_DEFAULT);
  pio_configure(XP_PIO, PIO_INPUT, XP_MASK, PIO_DEFAULT);
  pio_configure(XM_PIO, PIO_INPUT, XM_MASK, PIO_DEFAULT);
  delay_ms(5);
}

/*
*  xp to ground
*  ym to vcc
*  HI-Z xm e yp
*/
int readTouchZ(int rxplate) {
  pio_configure(XP_PIO, PIO_OUTPUT_0, XP_MASK, PIO_DEFAULT);
  pio_configure(YM_PIO, PIO_OUTPUT_1, YM_MASK, PIO_DEFAULT);
  
  pio_configure(YP_PIO, PIO_INPUT, YP_MASK, PIO_DEFAULT);
  pio_configure(XM_PIO, PIO_INPUT, XM_MASK, PIO_DEFAULT);
  afec_channel_enable(YP_AFEC, YP_CHANNEL);
  afec_channel_enable(XM_AFEC, XM_CHANNEL);
  
  delay_ms(5);
  int z1 = readXm();
  int z2 = readYp();
  configReadTouchX();
  int x  = readXm();
  
  float rtouch;
  rtouch = z2;
  rtouch /= z1;
  rtouch -= 1;
  rtouch *= x;
  rtouch *= rxplate;
  rtouch /= 1024;
  
  // aparecem alguns 0 do nada, evitar!
  if(rtouch < 100.0)
  rtouch = 1024.0;
  return(rtouch);
}

int readTouchX(void) {
  return(readYp());
}

int readTouchY(void) {
  return (readXm());
}

static void insert_sort(int array[], uint8_t size) {
  uint8_t j;
  int save;

  for (int i = 1; i < size; i++) {
    save = array[i];
    for (j = i; j >= 1 && save < array[j - 1]; j--)
    array[j] = array[j - 1];
    array[j] = save;
  }
}

void readRawPoint(int *x, int *y, int *z) {
  int samples[TOUCH_NUMSAMPLES];
  char valid = 0 ;
  
  configReadTouchX();
  for (int i=0; i<TOUCH_NUMSAMPLES; i++) {
    delay_us(1);
    samples[i] = readTouchX();
  }
  insert_sort(samples, TOUCH_NUMSAMPLES);
  *x = (samples[TOUCH_NUMSAMPLES / 2]);
  
  configReadTouchY();
  for (int i=0; i<TOUCH_NUMSAMPLES; i++) {
    samples[i] = readTouchY();
  }
  insert_sort(samples, TOUCH_NUMSAMPLES);
  *y = (samples[TOUCH_NUMSAMPLES / 2]);
  
  *z = readTouchZ(300);
}

// returns if valid touch
// checks Z pressure!
int pointToCoordinate(int x, int y, int z, int *px, int *py) {
  int xtemp, ytemp;
  
  xtemp = TOUCH_LCD_WIDTH*(x-TOUCH_X_240)/(TOUCH_X_0-TOUCH_X_240);
  if(xtemp > TOUCH_LCD_WIDTH) xtemp=TOUCH_LCD_WIDTH;
  if(xtemp < 0) xtemp=0;
  *py = xtemp;
    
  ytemp = TOUCH_LCD_HEIGHT - TOUCH_LCD_HEIGHT*(y-TOUCH_Y_0)/(TOUCH_Y_320-TOUCH_Y_0);
  if(ytemp > TOUCH_LCD_HEIGHT) ytemp=TOUCH_LCD_HEIGHT;
  if(ytemp < 0) ytemp=0;
  *px = ytemp;

  if(z < TOUCH_PRESSURE_DETECT) {
    return 1;
  }  
  return 0;
}

int readPoint(int *px, int *py){
  int x, y, z;
  readRawPoint(&x, &y, &z);
  return(pointToCoordinate(x,y,z, px, py));
}



