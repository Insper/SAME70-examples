#include "dataTypes.h"

void data_0(void){
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//350ns
	for(uint8_t i=0; i<0; i++){}			//350ns
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//350ns
	for(uint8_t i=0; i<0; i++){}			//350ns
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//350ns
	
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//800ns
	for(uint8_t i=0; i<1; i++){}			//800ns
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//800ns
}

void data_1(void){
	int i = 0;
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK; //700ns
	for(uint8_t i=0; i<20; i++){}			//700ns

	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	// 600ns
	for(uint8_t i=0; i<0; i++){}			// 600ns
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK; // 600ns
	for(uint8_t i=0; i<0; i++){}			// 600ns
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK; // 600ns
	for(uint8_t i=0; i<0; i++){}			// 600ns
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK; // 600ns
	for(uint8_t i=0; i<0; i++){}			// 600ns
}

void data_reset(void){
	
	pio_clear(PIN_DATA, PIN_DATA_IDX_MASK);
	delay_us(60);

}