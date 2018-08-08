/*
 * heartbeat_4md69.h
 *
 * Created: 17/03/2018 18:52:06
 *  Author: eduardomarossi
 */ 
#ifndef HEARTBEAT_4MD69_H_
#define HEARTBEAT_4MD69_H_

#include "fifo.h"
#include <asf.h>

#define MOVING_AVERAGE_SIZE 128
#define H4MD69_AFEC         AFEC0
#define H4MD69_AFEC_ID      ID_AFEC0
#define H4MD69_AFEC_CH      AFEC_CHANNEL_5 // Pin PB2
#define H4MD69_AFEC_CH_IR   AFEC_INTERRUPT_EOC_5
#define H4MD69_THRESHOLD    50

void h4d69_init(void);
void h4d69_enable_interrupt(void);
void h4d69_update(void);
void h4d69_convert(void);
int h4d69_get_value(void);
bool h4d69_has_beat(void);

#endif /* HEARTBEAT_4MD69_H_ */