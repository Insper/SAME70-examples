// Defines
#define TFMINI_BAUDRATE 115200
#define TFMINI_DEBUGMODE 1

// The frame size is nominally 9 characters, but we don't include the first two 0x59's marking the start of the frame
#define TFMINI_FRAME_SIZE 7

// Timeouts
#define TFMINI_MAXBYTESBEFOREHEADER 30
#define TFMINI_MAX_MEASUREMENT_ATTEMPTS 10

// States
#define READY 0
#define ERROR_SERIAL_NOHEADER 1
#define ERROR_SERIAL_BADCHECKSUM 2
#define ERROR_SERIAL_TOOMANYTRIES 3
#define MEASUREMENT_OK 10

void usart_put_char(Usart *usart, unsigned char n);