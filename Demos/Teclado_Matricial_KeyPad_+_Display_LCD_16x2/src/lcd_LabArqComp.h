/*
 * Data: 19/03/2019 
 * 5º Semestre - Engenharia da Computação - Insper
 * Marco Mello - marcoasma@insper.edu.br
 */

#ifndef lcd_LabArqComp_H_INCLUDED
#define lcd_LabArqComp_H_INCLUDED

#include "asf.h"

#define D4_PIO           PIOB
#define D4_PIO_ID        ID_PIOB
#define D4_PIO_IDX       0
#define D4_PIO_IDX_MASK  (1 << D4_PIO_IDX)

#define D5_PIO           PIOA
#define D5_PIO_ID        ID_PIOA
#define D5_PIO_IDX       3
#define D5_PIO_IDX_MASK  (1 << D5_PIO_IDX)

#define D6_PIO           PIOA
#define D6_PIO_ID        ID_PIOA
#define D6_PIO_IDX       4
#define D6_PIO_IDX_MASK  (1 << D6_PIO_IDX)

#define D7_PIO           PIOB
#define D7_PIO_ID        ID_PIOB
#define D7_PIO_IDX       1
#define D7_PIO_IDX_MASK  (1 << D7_PIO_IDX)

#define RS_PIO           PIOD
#define RS_PIO_ID        ID_PIOD
#define RS_PIO_IDX       21
#define RS_PIO_IDX_MASK  (1 << RS_PIO_IDX)

#define EN_PIO           PIOD
#define EN_PIO_ID        ID_PIOD
#define EN_PIO_IDX       25
#define EN_PIO_IDX_MASK  (1 << EN_PIO_IDX)

void Lcd4_Port(char a);
void Lcd4_Cmd(char a);
void Lcd4_Init();
void Lcd4_Clear();
void Lcd4_Set_Cursor(char a, char b);
void Lcd4_Write_Char(char a);
void Lcd4_Write_String(char *a);
void Lcd4_Shift_Right();
void Lcd4_Shift_Left();

#endif

