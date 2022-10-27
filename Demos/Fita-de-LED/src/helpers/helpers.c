#include "helpers.h"
/************************************************************************/
/* timers                                                              */
/************************************************************************/
void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
	uint32_t ul_div;
	uint32_t ul_tcclks;
	uint32_t ul_sysclk = sysclk_get_cpu_hz();

	pmc_enable_periph_clk(ID_TC);

	tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
	tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
	tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

	NVIC_SetPriority(ID_TC, 4);
	NVIC_EnableIRQ((IRQn_Type) ID_TC);
	tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);
}

// void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type) {
// 	/* Configura o PMC */
// 	pmc_enable_periph_clk(ID_RTC);
// 
// 	/* Default RTC configuration, 24-hour mode */
// 	rtc_set_hour_mode(rtc, 0);
// 
// 	/* Configura data e hora manualmente */
// 	rtc_set_date(rtc, t.year, t.month, t.day, t.week);
// 	rtc_set_time(rtc, t.hour, t.minute, t.seccond);
// 
// 	/* Configure RTC interrupts */
// 	NVIC_DisableIRQ(id_rtc);
// 	NVIC_ClearPendingIRQ(id_rtc);
// 	NVIC_SetPriority(id_rtc, 4);
// 	NVIC_EnableIRQ(id_rtc);
// 
// 	/* Ativa interrupcao via alarme */
// 	rtc_enable_interrupt(rtc,  irq_type);
// }
// 
// void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {
// 
// 	uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
// 
// 	rtt_sel_source(RTT, false);
// 	rtt_init(RTT, pllPreScale);
// 
// 	if (rttIRQSource & RTT_MR_ALMIEN) {
// 		uint32_t ul_previous_time;
// 		ul_previous_time = rtt_read_timer_value(RTT);
// 		while (ul_previous_time == rtt_read_timer_value(RTT));
// 		rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);
// 	}
// 
// 	/* config NVIC */
// 	NVIC_DisableIRQ(RTT_IRQn);
// 	NVIC_ClearPendingIRQ(RTT_IRQn);
// 	NVIC_SetPriority(RTT_IRQn, 4);
// 	NVIC_EnableIRQ(RTT_IRQn);
// 
// 	/* Enable RTT interrupt */
// 	if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
// 	rtt_enable_interrupt(RTT, rttIRQSource);
// 	else
// 	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
// 
// }
// 
// static uint32_t get_time_rtt() {
// 	return rtt_read_timer_value(RTT);
// }


/************************************************************************/
/* configs                                                              */
/************************************************************************/

void config_button(Pio *p_pio, const uint32_t ul_mask, uint32_t ul_id, void (*p_handler) (uint32_t, uint32_t), int it_fall, int filter) {
	pmc_enable_periph_clk(ul_id);
	filter ? pio_configure(p_pio, PIO_INPUT, ul_mask, PIO_PULLUP | PIO_DEBOUNCE) : pio_configure(p_pio, PIO_INPUT, ul_mask, PIO_DEFAULT);

	pio_set_debounce_filter(p_pio, ul_mask, 60);

	uint32_t but_attr = (it_fall ? PIO_IT_FALL_EDGE : PIO_IT_EDGE);
	pio_handler_set(p_pio,
	ul_id,
	ul_mask,
	but_attr,
	p_handler);

	pio_enable_interrupt(p_pio, ul_mask);
	pio_get_interrupt_status(p_pio);
	
	NVIC_EnableIRQ(ul_id);
	NVIC_SetPriority(ul_id, 4);
}