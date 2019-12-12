#include "asf.h"
#include "main.h"
#include <string.h>
#include "bsp/include/nm_bsp.h"
#include "driver/include/m2m_wifi.h"
#include "socket/include/socket.h"

/** IP address of host. */
uint32_t gu32HostIp = 0;

/** TCP client socket handlers. */
static SOCKET tcp_client_socket = -1;

/** Receive buffer definition. */
static uint8_t gau8ReceivedBuffer[MAIN_WIFI_M2M_BUFFER_SIZE] = {0};

/** Wi-Fi status variable. */
static bool gbConnectedWifi = false;

/** Get host IP status variable. */
/** Wi-Fi connection state */
static uint8_t wifi_connected;

/** Instance of HTTP client module. */
static bool gbHostIpByName = false;

/** TCP Connection status variable. */
static bool gbTcpConnection = false;

/** Server host name. */
static char server_host_name[] = MAIN_SERVER_NAME;

#define TASK_WIFI_STACK_SIZE            (4096/sizeof(portSTACK_TYPE))
#define TASK_WIFI_STACK_PRIORITY        (tskIDLE_PRIORITY)

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName);
extern void vApplicationIdleHook(void);
extern void vApplicationTickHook(void);
extern void vApplicationMallocFailedHook(void);
extern void xPortSysTickHandler(void);

/*****************************************
 * freeRTOS hook
 *****************************************/

/**
 * \brief Called if stack overflow during execution
 */

extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
		signed char *pcTaskName){
	printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	 * identify which task has overflowed its stack.
	 */
	for (;;) {
	}
}

extern void vApplicationIdleHook(void){
	
}

extern void vApplicationTickHook(void){}

extern void vApplicationMallocFailedHook(void){
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

	/* Force an assert. */
	configASSERT( ( volatile void * ) NULL );
}

/*****************************************
 * Funcões 
 ****************************************/

/**
 * \brief Configure UART console.
 */

static void configure_console(void){
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

int inet_aton(const char *cp, in_addr *ap){
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

static void resolve_cb(uint8_t *hostName, uint32_t hostIp){
	gu32HostIp = hostIp;
	gbHostIpByName = true;
	printf("resolve_cb: %s IP address is %d.%d.%d.%d\r\n\r\n", hostName,
			(int)IPV4_BYTE(hostIp, 0), (int)IPV4_BYTE(hostIp, 1),
			(int)IPV4_BYTE(hostIp, 2), (int)IPV4_BYTE(hostIp, 3));
}

static void socket_cb(SOCKET sock, uint8_t u8Msg, void *pvMsg) {
  
	/* Check for socket event on TCP socket. */
	if (sock == tcp_client_socket) {
    
		switch (u8Msg) {
		case SOCKET_MSG_CONNECT:
		{
      printf("socket_msg_connect\n"); 
			if (gbTcpConnection) {
				memset(gau8ReceivedBuffer, 0, sizeof(gau8ReceivedBuffer));
				sprintf((char *)gau8ReceivedBuffer, "%s", MAIN_PREFIX_BUFFER);

				tstrSocketConnectMsg *pstrConnect = (tstrSocketConnectMsg *)pvMsg;
				if (pstrConnect && pstrConnect->s8Error >= SOCK_ERR_NO_ERROR) {
          printf("send \n");
					send(tcp_client_socket, gau8ReceivedBuffer, strlen((char *)gau8ReceivedBuffer), 0);

					memset(gau8ReceivedBuffer, 0, MAIN_WIFI_M2M_BUFFER_SIZE);
					recv(tcp_client_socket, &gau8ReceivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
				} else {
					printf("socket_cb: connect error!\r\n");
					gbTcpConnection = false;
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
        printf(pstrRecv->pu8Buffer);
				
				memset(gau8ReceivedBuffer, 0, sizeof(gau8ReceivedBuffer));
				recv(tcp_client_socket, &gau8ReceivedBuffer[0], MAIN_WIFI_M2M_BUFFER_SIZE, 0);
			} else {
				printf("socket_cb: recv error!\r\n");
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

static void set_dev_name_to_mac(uint8_t *name, uint8_t *mac_addr) {
	/* Name must be in the format WINC1500_00:00 */
	uint16 len;

	len = m2m_strlen(name);
	if (len >= 5) {
		name[len - 1] = MAIN_HEX2ASCII((mac_addr[5] >> 0) & 0x0f);
		name[len - 2] = MAIN_HEX2ASCII((mac_addr[5] >> 4) & 0x0f);
		name[len - 4] = MAIN_HEX2ASCII((mac_addr[4] >> 0) & 0x0f);
		name[len - 5] = MAIN_HEX2ASCII((mac_addr[4] >> 4) & 0x0f);
	}
}

static void wifi_cb(uint8_t u8MsgType, void *pvMsg){
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
		
    /* Obtain the IP Address by network name */
		//gethostbyname((uint8_t *)server_host_name);
		break;
	}

	default:
	{
		break;
	}
	}
}

uint read_sdcard(void){
  char test_file_name[] = "0:config.txt";
  Ctrl_status status;
  FRESULT res;
  FATFS fs;
  FIL file_object;
  
    printf("Please plug an SD, MMC or SDIO card in slot.\n\r");

    /* Wait card present and ready */
    do {
      status = sd_mmc_test_unit_ready(0);
      if (CTRL_FAIL == status) {
        printf("Card install FAIL\n\r");
        printf("Please unplug and re-plug the card.\n\r");
        while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
          vTaskDelay(100);
        }
      }
      vTaskDelay(100);
    } while (CTRL_GOOD != status);

    printf("Mount disk (f_mount)...\r\n");
    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
    if (FR_INVALID_DRIVE == res) {
      printf("[FAIL] res %d\r\n", res);
      return 1;
    }

    test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
    res = f_open(&file_object,	(char const *)test_file_name,	FA_READ);
    if (res != FR_OK) {
      printf("[FAIL] res %d\r\n", res);
      return 1;
    }
        
    uint buffer[1024];
    
    f_gets(buffer, 1024, &file_object);
    printf("   %s",buffer);
    
    f_gets(buffer, 1024, &file_object);
    printf("   %s",buffer);
    
    f_close(&file_object);
    printf("Test is successful.\n");
    return 0;
}

void write_sdcard(void){
  char test_file_name[] = "0:config.txt";
  Ctrl_status status;
  FRESULT res;
  FATFS fs;
  FIL file_object;
  
   printf("Please plug an SD, MMC or SDIO card in slot.\n\r");

    /* Wait card present and ready */
    do {
      status = sd_mmc_test_unit_ready(0);
      if (CTRL_FAIL == status) {
        printf("Card install FAIL\n\r");
        printf("Please unplug and re-plug the card.\n\r");
        while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
                vTaskDelay(100);
        }
      }
      vTaskDelay(100);
    } while (CTRL_GOOD != status);

    printf("Mount disk (f_mount)...\r\n");
    memset(&fs, 0, sizeof(FATFS));
    res = f_mount(LUN_ID_SD_MMC_0_MEM, &fs);
    if (FR_INVALID_DRIVE == res) {
      printf("[FAIL] res %d\r\n", res);
      return 0;
    }

    test_file_name[0] = LUN_ID_SD_MMC_0_MEM + '0';
    res = f_open(&file_object,	(char const *)test_file_name, 	FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK) {
      printf("[FAIL] res %d\r\n", res);
      return 0;
    }
    
    f_puts("OLA \r\n  OI \r\n", &file_object);
    
    f_close(&file_object);
    printf("write is successful.\n\r");
    printf("Please unplug the card.\n\r");
    while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
      vTaskDelay(200);
    }
 }

/****************************************
 * TASK
 *****************************************/

static void task_wifi(void *pvParameters) {
	tstrWifiInitParam param;
	int8_t ret;
	uint8_t mac_addr[6];
	uint8_t u8IsMacAddrValid;
	struct sockaddr_in addr_in;
	
	/* Initialize the BSP. */
	nm_bsp_init();
	
	/* Initialize Wi-Fi parameters structure. */
	memset((uint8_t *)&param, 0, sizeof(tstrWifiInitParam));

	/* Initialize Wi-Fi driver with data and status callbacks. */
	param.pfAppWifiCb = wifi_cb;
	ret = m2m_wifi_init(&param);
	if (M2M_SUCCESS != ret) {
		printf("main: m2m_wifi_init call error!(%d)\r\n", ret);
		while (1) {
		}
	}
	
	/* Initialize socket module. */
	socketInit();

	/* Register socket callback function. */
	registerSocketCallback(socket_cb, resolve_cb);

	/* Connect to router. */
	printf("main: connecting to WiFi AP %s...\r\n", (char *)MAIN_WLAN_SSID);
	m2m_wifi_connect((char *)MAIN_WLAN_SSID, sizeof(MAIN_WLAN_SSID), MAIN_WLAN_AUTH, (char *)MAIN_WLAN_PSK, M2M_WIFI_CH_ALL);

	addr_in.sin_family = AF_INET;
	addr_in.sin_port = _htons(MAIN_SERVER_PORT);
	inet_aton(MAIN_SERVER_NAME, &addr_in.sin_addr);
	printf("Inet aton : %d", addr_in.sin_addr);
	
  while(1){
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
			  
			  if (connect(tcp_client_socket, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) != SOCK_ERR_NO_ERROR) {
				  close(tcp_client_socket);
				  tcp_client_socket = -1;
				  printf("error\n");
				  }else{
				  gbTcpConnection = true;
			  }
		  }
	  }
	  }
}

static void task_monitor(void *pvParameters){
	static portCHAR szList[256];
	UNUSED(pvParameters);

	for (;;) {
		printf("--- task ## %u", (unsigned int)uxTaskGetNumberOfTasks());
		vTaskList((signed portCHAR *)szList);
		printf(szList);
		vTaskDelay(1000);
	}
}

static void task_sdcard(void *pvParameters){
     /* Initialize SD MMC stack */
     printf("\x0C\n\r-- SD/MMC/SDIO Card Example on FatFs --\n\r");
     printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);
     
     sd_mmc_init();
     
     printf("----------------------------------- \n");
     printf("WRITE \n");
     printf("----------------------------------- \n");
     write_sdcard();
     
     while(1){
        printf("----------------------------------- \n");
        printf("READ \n");
        printf("----------------------------------- \n");
        read_sdcard();
        vTaskDelay(1000);
     }  
}

/****************************************
 * Main
 *****************************************/

int main(void){
	/* Initialize the board. */
	sysclk_init();
	board_init();

	/* Initialize the UART console. */
	configure_console();
  
	if (xTaskCreate(task_wifi, "Wifi", TASK_WIFI_STACK_SIZE, NULL,
	TASK_WIFI_STACK_PRIORITY, NULL) != pdPASS) {
		printf("Failed to create Wifi task\r\n");
	}

  if (xTaskCreate(task_sdcard, "sd", TASK_WIFI_STACK_SIZE, NULL,
  TASK_WIFI_STACK_PRIORITY, NULL) != pdPASS) {
    printf("Failed to create Wifi task\r\n");
  }


	vTaskStartScheduler();
	
	while(1) {};
	return 0;

}
