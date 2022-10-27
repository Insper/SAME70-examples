#ifndef CORES_H_
#define CORES_H_
#include "dataTypes.h"
#include "helpers/defines.h"

void colorTESTE(void);

void colorRED(void); 

void colorGREEN(void);

void colorBLUE(void);

void colorWHITE(void);

void colorBLACK(void);

void colorDEFINE(int8_t red, int8_t green, int8_t blue);

void setOneLED(uint32_t posicao, uint8_t estado);

void clearLEDs(void);
#endif /* CORES_H_ */