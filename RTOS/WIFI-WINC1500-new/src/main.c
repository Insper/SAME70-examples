#include "asf.h"
#include "main.h"
#include <string.h>
#include "bsp/include/nm_bsp.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"

#define STRING_EOL  "\r\n"
#define STRING_LINE "--------------------------------------------"STRING_EOL
#define STRING_HEADER "-- WINC1500 client example --"STRING_EOL	\
"-- "BOARD_NAME " --"STRING_EOL	\
"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL \
STRING_LINE

/************************************************************************/
/* WIFI                                                                 */
/************************************************************************/

/** IP address of host. */
uint32_t gu32HostIp = 0;

/** TCP client socket handlers. */
static SOCKET tcp_client_socket = -1;

/** Receive buffer definition. */
static uint8_t g_receivedBuffer[MAIN_WIFI_M2M_BUFFER_SIZE] = {0};
static uint8_t g_sendBuffer[MAIN_WIFI_M2M_BUFFER_SIZE] = {0};

/** Wi-Fi status variable. */
static bool gbConnectedWifi = false;

/** Get host IP status variable. */
/** Wi-Fi connection state */
static uint8_t wifi_connected;

/** Instance of HTTP client module. */
static bool gbHostIpByName = false;

/** TCP Connection status variable. */
static bool gbTcpConnection = false;
static bool gbTcpConnected = false;

/** Server host name. */
static char server_host_name[] = MAIN_SERVER_NAME;

/************************************************************************/
/* RTOS                                                                 */
/************************************************************************/

#define TASK_WIFI_STACK_SIZE      (4*4096/sizeof(portSTACK_TYPE))
#define TASK_WIFI_PRIORITY        (1)
#define TASK_PROCESS_STACK_SIZE   (1*4096/sizeof(portSTACK_TYPE))
#define TASK_PROCESS_PRIORITY     (0)

SemaphoreHandle_t xSemaphore;
QueueHandle_t xQueueMsg;

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);


/************************************************************************/
/* HOOKs                                                                */
/************************************************************************/

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName){
  printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
  for (;;) {  }
}

extern void vApplicationIdleHook(void){}

extern void vApplicationTickHook(void){}

extern void vApplicationMallocFailedHook(void){
  configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

/**
* \brief Configure UART console.
*/
static void configure_console(void)
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

/************************************************************************/
/* callbacks                                                            */
/************************************************************************/

/**
* \brief Callback function of IP address.
*
* \param[in] hostName Domain name.
* \param[in] hostIp Server IP.
*
* \return None.
*/
static void resolve_cb(uint8_t *hostName, uint32_t hostIp)
{
  gu32HostIp = hostIp;
  gbHostIpByName = true;
  printf("resolve_cb: %s IP address is %d.%d.%d.%d\r\n\r\n", hostName,
  (int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
  (int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
}

/**
* \brief Callback function of TCP client socket.
*
* \param[in] sock socket handler.
* \param[in] u8Msg Type of Socket notification
* \param[in] pvMsg A structure contains notification informations.
*
* \return None.
*/
static void socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg)
{
  /* Check for socket event on TCP socket. */
  if (sock == tcp_client_socket) {

    switch (u8Msg) {
      case SOCKET_MSG_CONNECT:
      {
        printf("socket_msg_connect\n");
        if (gbTcpConnection) {
          tstrSocketConnectMsg *pstrConnect = (tstrSocketConnectMsg *)pvMsg;
          if (pstrConnect && pstrConnect->s8Error >= SOCK_ERR_NO_ERROR) {
            printf("socket_cb: connect ok \n");
            gbTcpConnected = true;
            } else {
            printf("socket_cb: connect error!\r\n");
            gbTcpConnection = false;
            gbTcpConnected = false;
            close(tcp_client_socket);
            tcp_client_socket = -1;
          }
        }
      }
      break;

      case SOCKET_MSG_RECV:
      {
        char *pcIndxPtr;
        char *pcEndPtr;

        tstrSocketRecvMsg *pstrRecv = (tstrSocketRecvMsg *)pvMsg;
        if (pstrRecv && pstrRecv->s16BufferSize > 0) {
          xQueueSend(xQueueMsg, &pstrRecv, 10);
          xSemaphoreGive( xSemaphore );
          }  else {
          //printf("socket_cb: recv error!\r\n");
          close(tcp_client_socket);
          tcp_client_socket = -1;
        }
      }
      break;

      default:
      break;
    }
  }
}

/**
* \brief Callback to get the Wi-Fi status update.
*
* \param[in] u8MsgType Type of Wi-Fi notification.
* \param[in] pvMsg A pointer to a buffer containing the notification parameters.
*
* \return None.
*/
static void wifi_cb(uint8_t u8MsgType, void *pvMsg)
{
  switch (u8MsgType) {
    case M2M_WIFI_RESP_CON_STATE_CHANGED:
    {
      tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)pvMsg;
      if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) {
        printf("wifi_cb: M2M_WIFI_CONNECTED\r\n");
        m2m_wifi_request_dhcp_client();
        } else if (pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED) {
        printf("wifi_cb: M2M_WIFI_DISCONNECTED\r\n");
        gbConnectedWifi = false;
        wifi_connected = 0;
      }

      break;
    }

    case M2M_WIFI_REQ_DHCP_CONF:
    {
      uint8_t *pu8IPAddress = (uint8_t *)pvMsg;
      printf("wifi_cb: IP address is %u.%u.%u.%u\r\n",
      pu8IPAddress[0], pu8IPAddress[1], pu8IPAddress[2], pu8IPAddress[3]);
      wifi_connected = M2M_WIFI_CONNECTED;
      break;
    }

    default:
    {
      break;
    }
  }
}

/************************************************************************/
/* TASKS                                                                */
/************************************************************************/

static void task_process(void *pvParameters) {

  printf("task process created \n");
  vTaskDelay(1000);
  
  uint msg_counter = 0;
  tstrSocketRecvMsg *p_recvMsg;
  while(1){
    
    // aguarda task_wifi conectar no wifi e socket estar pronto
    while(gbTcpConnection == false && tcp_client_socket >= 0){
      vTaskDelay(10);
    }
    
    // faz nova requisicao get ao socket
    // e espera por duas mensagens!
    // comunicacao tcp/ip possui ack
    if(msg_counter == 0){
      // faz nova requisicao
      msg_counter = 2;
      sprintf((char *)g_sendBuffer, "%s", MAIN_PREFIX_BUFFER);
      send(tcp_client_socket, g_sendBuffer, strlen((char *)g_sendBuffer), 0);
      
      // requisita primeira primeira resposta!
      memset(g_receivedBuffer, 0, MAIN_WIFI_M2M_BUFFER_SIZE);
      recv(tcp_client_socket, &g_receivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
    }
    
    if(xQueueReceive(xQueueMsg, &p_recvMsg, 5000) == pdTRUE){
      printf(STRING_LINE);
      printf(p_recvMsg->pu8Buffer);
      printf(STRING_EOL);  printf(STRING_LINE);
      msg_counter--;
      // ainda espera por mais dados?
      if(msg_counter > 0 ){
        memset(g_receivedBuffer, 0, MAIN_WIFI_M2M_BUFFER_SIZE);
        recv(tcp_client_socket, &g_receivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
      }
    }
    else{
      msg_counter = 0;
    }
    vTaskDelay(100);
  }
}

static void task_wifi(void *pvParameters) {
  tstrWifiInitParam param;
  int8_t ret;
  struct sockaddr_in addr_in;

  xSemaphore = xSemaphoreCreateCounting(20,0);
  xQueueMsg = xQueueCreate(10, sizeof(tstrSocketRecvMsg));

  /* Initialize the BSP. */
  nm_bsp_init();

  /* Initialize Wi-Fi parameters structure. */
  memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));

  /* Initialize Wi-Fi driver with data and status callbacks. */
  param.pfAppWifiCb = wifi_cb;
  ret = m2m_wifi_init(&param);
  if (M2M_SUCCESS != ret) {
    printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
    while (1) { }
  }

  /* Initialize socket module. */
  socketInit();

  /* Register socket callback function. */
  registerSocketCallback(socket_cb, resolve_cb);

  /* Connect to router. */
  printf("main: connecting to WiFi AP %s...\r\n", (char *)MAIN_WLAN_SSID);
  m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);

  /* formata ip */
  addr_in.sin_family = AF_INET;
  addr_in.sin_port = _htons(MAIN_SERVER_PORT);
  inet_aton(MAIN_SERVER_NAME, &addr_in.sin_addr);

  while(1){
    if(xSemaphoreTake(xSemaphore, 500)){
      m2m_wifi_handle_events(NULL);

      if (wifi_connected == M2M_WIFI_CONNECTED) {
        /* Open client socket. */
        if (tcp_client_socket < 0) {
          printf("socket init \n");
          if ((tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("main: failed to create TCP client socket error!\r\n");
            continue;
          }

          /* Connect server */
          printf("socket connecting\n");
          if (connect(tcp_client_socket, (struct sockaddr *)&addr_in,
          sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) {
            close(tcp_client_socket);
            tcp_client_socket = -1;
            printf("main: error connect to socket\n");
            }else{
            gbTcpConnection = true;
          }
        }
      }
    }
  }
}

int main(void)
{
  /* Initialize the board. */
  sysclk_init();
  board_init();

  /* Initialize the UART console. */
  configure_console();
  printf(STRING_HEADER);

  xTaskCreate(task_wifi, "Wifi", TASK_WIFI_STACK_SIZE, NULL, TASK_WIFI_PRIORITY, NULL);
  xTaskCreate( task_process, "process", TASK_PROCESS_STACK_SIZE, NULL, TASK_PROCESS_PRIORITY,  NULL );

  vTaskStartScheduler();

  while(1) {};
  return 0;

}
