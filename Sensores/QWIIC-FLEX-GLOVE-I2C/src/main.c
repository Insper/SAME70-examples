/**
*  Computação Embarcada
*  Insper
*  Rafael Corsi - rafael.corsi@insper.edu.br
*  Código exemplo uso I2C
*  Abril - 2017
*
*  Bug conhecido : spi read deve ser executado duas vezes ?
*      - funcao afetada : mcu6050_i2c_bus_read()
*
*  Conectar :
*            MCU  |  SAME70-XPLD |
*           ----------------------
*            SDA  |   PD-27      |
*            SCL  |   PD-28      |
*            GND  |   EXT2-19    |
*            VCC  |   EXT2-20    |
*
* ref [1] http://playground.arduino.cc/Main/MPU-6050#short
* ref [2] https://github.com/jarzebski/Arduino-MPU6050/blob/master/MPU6050.cpp
*/


#include "asf.h"
#include "mcu6050.h"

/**
*
*/
#define TWIHS_ADS1015_ID    ID_TWIHS2
#define TWIHS_ADS1015       TWIHS2

/************************************************************************/
/* asd1015                                                              */
/************************************************************************/

#define ADS1015_ADDRESS_GND 0x48 //7-bit unshifted default I2C Address
#define ADS1015_ADDRESS_VDD 0x49
#define ADS1015_ADDRESS_SDA 0x4A
#define ADS1015_ADDRESS_SCL 0x4B

#define I2C_SPEED_STANDARD        100000
#define I2C_SPEED_FAST            400000

//Register addresses
#define ADS1015_DELAY                (1)

//Pointer Register
#define ADS1015_POINTER_CONVERT      (0x00)
#define ADS1015_POINTER_CONFIG       (0x01)

#define ADS1015_CONFIG_OS_NO         (0x8000)
#define ADS1015_CONFIG_OS_SINGLE     (0x8000)
#define ADS1015_CONFIG_OS_READY      (0x0000)
#define ADS1015_CONFIG_OS_NOTREADY   (0x8000)

#define ADS1015_CONFIG_MODE_CONT     (0x0000)
#define ADS1015_CONFIG_MODE_SINGLE   (0x0100)

#define ADS1015_CONFIG_MUX_SINGLE_0  (0x4000)
#define ADS1015_CONFIG_MUX_SINGLE_1  (0x5000)
#define ADS1015_CONFIG_MUX_SINGLE_2  (0x6000)
#define ADS1015_CONFIG_MUX_SINGLE_3  (0x7000)

#define ADS1015_CONFIG_RATE_128HZ    (0x0000)
#define ADS1015_CONFIG_RATE_250HZ    (0x0020)
#define ADS1015_CONFIG_RATE_490HZ    (0x0040)
#define ADS1015_CONFIG_RATE_920HZ    (0x0060)
#define ADS1015_CONFIG_RATE_1600HZ   (0x0080)
#define ADS1015_CONFIG_RATE_2400HZ   (0x00A0)
#define ADS1015_CONFIG_RATE_3300HZ   (0x00C0)

#define ADS1015_CONFIG_PGA_MASK      (0X0E00)
#define ADS1015_CONFIG_PGA_2/3       (0X0000)  // +/- 6.144v
#define ADS1015_CONFIG_PGA_1         (0X0200)  // +/- 4.096v
#define ADS1015_CONFIG_PGA_2         (0X0400)  // +/- 2.048v
#define ADS1015_CONFIG_PGA_4         (0X0600)  // +/- 1.024v
#define ADS1015_CONFIG_PGA_8         (0X0800)  // +/- 0.512v
#define ADS1015_CONFIG_PGA_16        (0X0A00)  // +/- 0.256v


/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

/**
*  Toggle pin controlado pelo PIO (out)
*/
/**
* \brief Configure UART console.
* BaudRate : 115200
* 8 bits
* 1 stop bit
* sem paridade
*/
static void configure_console(void)
{

	/* Configura USART1 Pinos */
	sysclk_enable_peripheral_clock(ID_PIOB);
	sysclk_enable_peripheral_clock(ID_PIOA);
	pio_set_peripheral(PIOB, PIO_PERIPH_D, PIO_PB4);  // RX
	pio_set_peripheral(PIOA, PIO_PERIPH_A, PIO_PA21); // TX
	MATRIX->CCFG_SYSIO |= CCFG_SYSIO_SYSIO4;
	
	const usart_serial_options_t uart_serial_options = {
		.baudrate   = CONF_UART_BAUDRATE,
		.charlength = CONF_UART_CHAR_LENGTH,
		.paritytype = CONF_UART_PARITY,
		.stopbits   = CONF_UART_STOP_BITS,
	};

	/* Configure console UART. */
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

/*
*  \Brief: The function is used as I2C bus init
*/
void ads1015_i2c_bus_init(void)
{
	twihs_options_t bno055_option;
	pmc_enable_periph_clk(TWIHS_ADS1015_ID);

	/* Configure the options of TWI driver */
	bno055_option.master_clk = sysclk_get_cpu_hz();
	bno055_option.speed      = 40000;
	twihs_master_init(TWIHS_ADS1015, &bno055_option);
}

int configSensor(uint8_t channel){
		twihs_packet_t packet_tx;
	
	uint16_t config = ADS1015_CONFIG_OS_SINGLE | ADS1015_CONFIG_MODE_CONT | ADS1015_CONFIG_RATE_1600HZ | ADS1015_CONFIG_PGA_2/3 | ADS1015_CONFIG_MUX_SINGLE_0;
	//config |= ADS1015_CONFIG_PGA_2/3;
	
	switch (channel) {
		case (0):
		config |= ADS1015_CONFIG_MUX_SINGLE_0;
		break;
		case (1):
		config |= ADS1015_CONFIG_MUX_SINGLE_1;
		break;
		case (2):
		config |= ADS1015_CONFIG_MUX_SINGLE_2;
		break;
		case (3):
		config |= ADS1015_CONFIG_MUX_SINGLE_3;
		break;
	}
	//config = 0xc080;
	//config |= ADS1015_CONFIG_MUX_SINGLE_0;
	
	char tx[3];
	tx[0] = ADS1015_POINTER_CONFIG;
	tx[1] = config >> 8;
	tx[2] = config & 0xFF;
	packet_tx.buffer = &tx;
	packet_tx.length = 3;
	packet_tx.chip = (uint32_t) ADS1015_ADDRESS_GND;
	packet_tx.addr[0] = 0;
	packet_tx.addr_length = 0;

 	/* Perform a master write access */
 	if (twihs_master_write(TWIHS2, &packet_tx)){
		printf("falha na escrita \n");
	}
	
	delay_ms(ADS1015_DELAY);
}

int readChannel(uint8_t channel, uint8_t *rvalue){
	twihs_packet_t packet_tx, packet_rx;
	char tx[32];
	char rx[32];

	//tx[0] = ADS1015_POINTER_CONFIG;
	tx[0] = 0;
	packet_tx.buffer = &tx;
	packet_tx.length = 1;
	packet_tx.chip = (uint32_t) ADS1015_ADDRESS_GND;
	packet_tx.addr[0] = 0;
	packet_tx.addr_length = 0;
	
 	if (twihs_master_write(TWIHS2, &packet_tx)){
		printf("falha na escrita \n");
	}
	 
	packet_rx.buffer = &rx;
	packet_rx.length = 2;
	packet_rx.chip = (uint32_t) ADS1015_ADDRESS_GND;
	packet_rx.addr[0] = 0;
	packet_rx.addr_length = 0;
	if (twihs_master_read(TWIHS2, &packet_rx)){
		printf("falha na leitura \n");
	}
	printf("Channel%u: ", channel);
	printf("%d  \r\n", (rx[0] << 8 | rx[1]) >> 4);
	
}

/************************************************************************/
/* Main Code	                                                        */
/************************************************************************/
int main(void){
	
	/* buffer para recebimento de dados */
	uint8_t bufferRX[100];
	uint8_t bufferTX[100];
	
	uint8_t rtn;

	/* Initialize the SAM system */
	sysclk_init();
	board_init();
	
	/** Enable TWIHS port. */
	pmc_enable_periph_clk(ID_PIOD);
	pio_set_peripheral(PIOD, PIO_TYPE_PIO_PERIPH_C, 1 << 28);
	pio_set_peripheral(PIOD, PIO_TYPE_PIO_PERIPH_C, 1 << 27);
	
	/* Disable the watchdog */
	WDT->WDT_MR = WDT_MR_WDDIS;
	
	/* Inicializa com serial com PC*/
	configure_console();
	
	/* Inicializa funcao de delay */
	delay_init( sysclk_get_cpu_hz());
	
	/************************************************************************/
	/* MPU                                                                  */
	/************************************************************************/
	
	/* Inicializa i2c */
	printf("Inicializando bus i2c \n");
	ads1015_i2c_bus_init();
	
	// verifica se o chip está conectado
	while (twihs_probe(TWIHS2, ADS1015_ADDRESS_GND)) {
		printf("Chip nao encontrado, verifique conexao \n");
	}
	printf("Conexao i2c ok, inicializando ADS1015");
	
	configSensor(0);
	configSensor(1);
	
	uint8_t d0, d1;
	while(1) {
		//dedo direito
		configSensor(0);
		readChannel(0, &d0);
		delay_us(100);
		
		//dedo esquerdo
		configSensor(1);
		readChannel(1, &d1);
		delay_ms(100);
	}
}
