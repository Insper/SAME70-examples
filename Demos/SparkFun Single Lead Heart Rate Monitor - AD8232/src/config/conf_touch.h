/*
 * conf_touch.h
 *
 * Created: 04/02/2021 12:34:21
 *  Author: RafaelCF4
 */ 


#ifndef CONF_TOUCH_H_
#define CONF_TOUCH_H_

//x-
#define XM_AFEC     AFEC1
#define XM_AFEC_ID  ID_AFEC1
#define XM_CHANNEL  1

#define XM_PIO     PIOC
#define XM_PIO_ID  ID_PIOC
#define XM_MASK    PIO_PC13

// y-
#define YP_AFEC     AFEC0
#define YP_AFEC_ID  ID_AFEC0
#define YP_CHANNEL  0

#define YP_PIO     PIOD
#define YP_PIO_ID  ID_PIOD
#define YP_MASK    PIO_PD30

// x+
#define XP_PIO      PIOC
#define XP_PIO_ID   ID_PIOC
#define XP_MASK     PIO_PC19

// y+
#define YM_PIO      PIOD
#define YM_PIO_ID   ID_PIOD
#define YM_MASK     PIO_PD11

#define TOUCH_NUMSAMPLES  64
#define TOUCH_LCD_WIDTH   240
#define TOUCH_LCD_HEIGHT  320
#define TOUCH_X_240       750
#define TOUCH_X_0         3300
#define TOUCH_Y_320       3400
#define TOUCH_Y_0         620
#define TOUCH_PRESSURE_DETECT 500

#endif /* CONF_TOUCH_H_ */