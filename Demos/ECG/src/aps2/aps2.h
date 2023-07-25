/*
 * aps2.h
 *
 * Created: 10/05/2021 11:07:47
 *  Author: RafaelCF4
 */ 


#ifndef APS2_H_
#define APS2_H_

#include <asf.h>
//#include "ecg.h"
#include "arm_math.h"

extern QueueHandle_t xQueueOx;
extern TimerHandle_t xTimer;
extern volatile int g_ecgCnt;
extern volatile int g_ecgDelayCnt;
extern volatile int g_ecgDelayValue;
extern const int ecg[];
extern const int g_ecgSize;

#define ECG_CHANGE_TIME 10000

//! DAC channel used for test
#define DACC_CHANNEL        0 // (PB13)
//! DAC register base for test
#define DACC_BASE           DACC
//! DAC ID for test
#define DACC_ID             ID_DACC

#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) | DACC_ACR_IBCTLCH1(0x02))

#define DAC_FS 2000

void TC0_Handler(void);
void task_aps2(void *pvParameters);

#endif /* APS2_H_ */