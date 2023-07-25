/*
 * IncFile1.h
 *
 * Created: 21/05/2019 09:51:36
 *  Author: MarcoASMA
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_

///////WS2811/////////
//****************************************
/*
void data_0(void){
	
    PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK; //500ns
    for(uint8_t i=0; i<1; i=i+1){}			//500ns
	for(uint8_t i=0,j=1; i<0; i=i+j){}		//500ns
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//500ns
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//500ns
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//500ns
			
	
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//2us 
	for(uint8_t i=0; i<10; i++){}			//2us
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//2us
    PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//2us
}

void data_1(void){
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	
	for(uint8_t i=0; i<1; i=i+1){}			//1.2us
	for(uint8_t i=0,j=1; i<0; i=i+j){}		//1.2us
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//1.2us
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//1.2us
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//1.2us
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK; //1.2us
	for(uint8_t i=0; i<1; i=i+1){}			//1.2us
	for(uint8_t i=0,j=1; i<0; i=i+j){}		//1.2us
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//1.2us
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//1.2us
	PIN_DATA->PIO_SODR = PIN_DATA_IDX_MASK;	//1.2us
	  

	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//1.3us
	for(uint8_t i=0; i<10; i++){}			//1.3us
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//1.3us
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//1.3us
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//1.3us
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//1.3us
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//1.3us
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//1.3us
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//1.3us
	PIN_DATA->PIO_CODR = PIN_DATA_IDX_MASK;	//1.3us
	
}
*/
///////WS2811/////////
//****************************************





///////WS2812/////////
//****************************************

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
//****************************************
///////WS2812/////////



void data_reset(void){
	
	pio_clear(PIN_DATA, PIN_DATA_IDX_MASK);
	delay_us(60);

}



#endif /* INCFILE1_H_ */