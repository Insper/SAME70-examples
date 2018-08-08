/*
 * heartbeat_4md69.c
 *
 * Created: 17/03/2018 18:51:48
 *  Author: eduardomarossi
 */ 

#include "heartbeat_4md69.h"

fifo_desc_t fifo_afec;
uint16_t    fifo_afec_data[MOVING_AVERAGE_SIZE];

/** The conversion data is done flag */
volatile bool is_conversion_done = false;

/** The conversion data value */
volatile uint32_t g_ul_value = 0;

/**
 * \brief AFEC interrupt callback function.
 */
static void afec_temp_sensor_end_conversion(void)
{
	g_ul_value = afec_channel_get_value(H4MD69_AFEC, H4MD69_AFEC_CH);
	is_conversion_done = true;
}


void h4d69_init(void) {
	afec_enable(H4MD69_AFEC);
	struct afec_config afec_cfg;
	afec_get_config_defaults(&afec_cfg);
	afec_init(H4MD69_AFEC, &afec_cfg);
	afec_set_trigger(H4MD69_AFEC, AFEC_TRIG_SW);

	struct afec_ch_config afec_ch_cfg;
	afec_ch_get_config_defaults(&afec_ch_cfg);
	afec_ch_cfg.gain = AFEC_GAINVALUE_0;
    fifo_init(&fifo_afec, &fifo_afec_data, MOVING_AVERAGE_SIZE);
	afec_ch_set_config(H4MD69_AFEC, H4MD69_AFEC_CH, &afec_ch_cfg);
	afec_channel_set_analog_offset(AFEC0, AFEC_CHANNEL_5, 0x200); // internal ADC offset is 0x200, it should cancel it and shift to 0
	afec_set_callback(H4MD69_AFEC, H4MD69_AFEC_CH_IR, afec_temp_sensor_end_conversion, 1);
	afec_channel_enable(H4MD69_AFEC, H4MD69_AFEC_CH);
	NVIC_SetPriority(AFEC0_IRQn, 10);
	
}

void h4d69_enable_interrupt(void) {
	afec_enable_interrupt(H4MD69_AFEC, H4MD69_AFEC_CH_IR);
	NVIC_EnableIRQ(AFEC0_IRQn);
}

void h4d69_update(void) {
	if(is_conversion_done == true) {
		if(fifo_is_full(&fifo_afec)) {
			fifo_pull_uint16_nocheck(&fifo_afec);
		}
				
		fifo_push_uint16_nocheck(&fifo_afec, (uint32_t) g_ul_value);
				
		is_conversion_done = false;
	}
}

void h4d69_convert(void) {
	if(!is_conversion_done) {
		afec_start_software_conversion(AFEC0);
	}
}

int h4d69_get_value(void) { 
	if(fifo_is_full(&fifo_afec)) {
		return g_ul_value - fifo_average_uint16(&fifo_afec);
	}
	return 0;
}

bool h4d69_has_beat(void) {
	if(h4d69_get_value() > H4MD69_THRESHOLD) {
		return true;
	}
	return false;
}