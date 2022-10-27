#ifndef DEFINES_H_   /* Include guard */
#define DEFINES_H_

#include <asf.h>

// Número de LEDs na fita
#define LEDS_NUMBER 44

//MAIN BUT
#define BUT_PIO PIOA
#define BUT_PIO_ID ID_PIOA
#define BUT_PIO_PIN 11
#define BUT_IDX_MASK (1 << BUT_PIO_PIN)

#define LED1_PIO PIOA
#define LED1_PIO_ID ID_PIOA
#define LED1_IDX 0
#define LED1_IDX_MASK (1 << LED1_IDX)

#define LED2_PIO PIOC
#define LED2_PIO_ID ID_PIOC
#define LED2_IDX 30
#define LED2_IDX_MASK (1 << LED2_IDX)

#define LED3_PIO PIOB
#define LED3_PIO_ID ID_PIOB
#define LED3_IDX 2
#define LED3_IDX_MASK (1 << LED3_IDX)

//BUTTONS OLED1
#define BUT1_PIO PIOD
#define BUT1_PIO_ID	ID_PIOD
#define BUT1_PIO_IDX 28
#define BUT1_IDX_MASK (1 << BUT1_PIO_IDX)

#define BUT2_PIO PIOC
#define BUT2_PIO_ID	ID_PIOC
#define BUT2_PIO_IDX 31
#define BUT2_IDX_MASK (1 << BUT2_PIO_IDX)

#define BUT3_PIO PIOA
#define BUT3_PIO_ID	ID_PIOA
#define BUT3_PIO_IDX 19
#define BUT3_IDX_MASK (1 << BUT3_PIO_IDX)

#define LED_PIO   PIOC
#define LED_PIO_ID    ID_PIOC
#define LED_PIO_IDX   8
#define LED_IDX_MASK (1u << LED_PIO_IDX)

//DATA PIN
#define PIN_DATA      PIOD
#define PIN_DATA_ID   ID_PIOD
#define PIN_DATA_IDX  30
#define PIN_DATA_IDX_MASK (1 << PIN_DATA_IDX)

#endif