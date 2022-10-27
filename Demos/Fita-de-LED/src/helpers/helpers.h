#ifndef HELPERS_H_   /* Include guard */
#define HELPERS_H_
#include <stdint.h>
#include <asf.h>


// typedef struct  {
// 	uint32_t year;
// 	uint32_t month;
// 	uint32_t day;
// 	uint32_t week;
// 	uint32_t hour;
// 	uint32_t minute;
// 	uint32_t seccond;
// } calendar;

/************************************************************************/
/* timers                                                              */
/************************************************************************/
void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq);

// void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type);
// 
// void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource);
// static uint32_t get_time_rtt();

/************************************************************************/
/* configs                                                              */
/************************************************************************/

void config_button(Pio *p_pio, const uint32_t ul_mask, uint32_t ul_id, void (*p_handler) (uint32_t, uint32_t), int it_rise, int filter);

#endif // HELPERS_H_