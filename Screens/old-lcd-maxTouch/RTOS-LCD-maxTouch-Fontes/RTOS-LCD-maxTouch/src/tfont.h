/*
 * tfont.h
 *
 * Created: 05/03/2019 17:20:03
 *  Author: eduardo
 */ 


#ifndef TFONT_H_
#define TFONT_H_

typedef struct {
	const uint8_t *data;
	int width;
	int height;
	int other;
} tImage;

 typedef struct {
	 long int code;
	 const tImage *image;
 } tChar;
 
 typedef struct {
	 int length;
	 const tChar *chars;
	 char start_char;
	 char end_char;
 } tFont;
 


#endif /* TFONT_H_ */