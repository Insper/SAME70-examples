/*
 * util.c
 *
 * Created: 12/05/2020 22:32:18
 *  Author: RafaelCF4
 */ 

#include "util.h"

/**
* \brief Configure UART console.
*/
void configure_console(void)
{
  const usart_serial_options_t uart_serial_options = {
    .baudrate =		CONF_UART_BAUDRATE,
    .charlength =	CONF_UART_CHAR_LENGTH,
    .paritytype =	CONF_UART_PARITY,
    .stopbits =		CONF_UART_STOP_BITS,
  };

  /* Configure UART console. */
  sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
  stdio_serial_init(CONF_UART, &uart_serial_options);
}

/*
* corsi
* Check whether "cp" is a valid ascii representation
* of an Internet address and convert to a binary address.
* Returns 1 if the address is valid, 0 if not.
* This replaces inet_addr, the return value from which
* cannot distinguish between failure and a local broadcast address.
* http://www.cs.cmu.edu/afs/cs/academic/class/15213-f00/unpv12e/libfree/inet_aton.c
*/
int inet_aton(const char *cp, in_addr *ap)
{
  int dots = 0;
  register u_long acc = 0, addr = 0;

  do {
    register char cc = *cp;

    switch (cc) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      acc = acc * 10 + (cc - '0');
      break;

      case '.':
      if (++dots > 3) {
        return 0;
      }
      /* Fall through */

      case '\0':
      if (acc > 255) {
        return 0;
      }
      addr = addr << 8 | acc;
      acc = 0;
      break;

      default:
      return 0;
    }
  } while (*cp++) ;

  /* Normalize the address */
  if (dots < 3) {
    addr <<= 8 * (3 - dots) ;
  }

  /* Store it if requested */
  if (ap) {
    ap->s_addr = _htonl(addr);
  }

  return 1;
}