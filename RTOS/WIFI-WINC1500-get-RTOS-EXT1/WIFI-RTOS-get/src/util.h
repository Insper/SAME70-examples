/*
 * util.h
 *
 * Created: 12/05/2020 22:32:03
 *  Author: RafaelCF4
 */ 


#ifndef UTIL_H_
#define UTIL_H_

#include "conf_uart_serial.h"
#include "asf.h"
#include "socket/include/socket.h"

/************************************************************************/
/* debug                                                                */
/************************************************************************/

#define STRING_EOL  "\r\n"
#define STRING_LINE "--------------------------------------------"STRING_EOL
#define STRING_HEADER "-- WINC1500 client example --"STRING_EOL	\
"-- "BOARD_NAME " --"STRING_EOL	\
"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL \
STRING_LINE

void configure_console(void);
int inet_aton(const char *cp, in_addr *ap);

#endif /* UTIL_H_ */