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
 *            SDA  |   EXT2-11    |  
 *            SCL  |   EXT2-12    |
 *            GND  |   EXT2-19    |
 *            VCC  |   EXT2-20    | 
 * 
 * ref [1] http://playground.arduino.cc/Main/MPU-6050#short
 * ref [2] https://github.com/jarzebski/Arduino-MPU6050/blob/master/MPU6050.cpp
 */

 
#include "asf.h"
#include "mcu6050.h"

/**
 * LEDs
 */
#define LED_PIO_ID		  ID_PIOC
#define LED_PIO         PIOC
#define LED_PIN		      8
#define LED_PIN_MASK    (1<<LED_PIN)

/**
 * Botão
 */
#define BUT_PIO_ID            ID_PIOA
#define BUT_PIO               PIOA
#define BUT_PIN		            11
#define BUT_PIN_MASK          (1 << BUT_PIN)
#define BUT_DEBOUNCING_VALUE  79

/** 
 * 
 */
#define TWIHS_MCU6050_ID    ID_TWIHS0 
#define TWIHS_MCU6050       TWIHS0  


/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/

float rangePerDigit ; // 2G
//const float rangePerDigit = 9.80665f ; // 2G

volatile uint8_t flag_led0 = 1;

 int16_t  accX, accY, accZ;
volatile uint8_t  accXHigh, accYHigh, accZHigh;
volatile uint8_t  accXLow,  accYLow,  accZLow;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

void BUT_init(void);
void LED_init(int estado);
void pin_toggle(Pio *pio, uint32_t mask);

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

/**
 *  Handle Interrupcao botao 1
 */
static void Button1_Handler(uint32_t id, uint32_t mask)
{
  pin_toggle(PIOD, (1<<28));
  pin_toggle(LED_PIO, LED_PIN_MASK);
}


/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

/** 
 *  Toggle pin controlado pelo PIO (out)
 */
void pin_toggle(Pio *pio, uint32_t mask){
   if(pio_get_output_data_status(pio, mask))
    pio_clear(pio, mask);
   else
    pio_set(pio,mask);
}

/**
 * @Brief Inicializa o pino do BUT
 */
void BUT_init(void){
    /* config. pino botao em modo de entrada */
    pmc_enable_periph_clk(BUT_PIO_ID);
    pio_set_input(BUT_PIO, BUT_PIN_MASK, PIO_PULLUP | PIO_DEBOUNCE);
    
    /* config. interrupcao em borda de descida no botao do kit */
    /* indica funcao (but_Handler) a ser chamada quando houver uma interrupção */
    pio_enable_interrupt(BUT_PIO, BUT_PIN_MASK);
    pio_handler_set(BUT_PIO, BUT_PIO_ID, BUT_PIN_MASK, PIO_IT_FALL_EDGE, Button1_Handler);
    
    /* habilita interrupçcão do PIO que controla o botao */
    /* e configura sua prioridade                        */
    NVIC_EnableIRQ(BUT_PIO_ID);
    NVIC_SetPriority(BUT_PIO_ID, 1);
};

/**
 * @Brief Inicializa o pino do LED
 */
void LED_init(int estado){
    pmc_enable_periph_clk(LED_PIO_ID);
    pio_set_output(LED_PIO, LED_PIN_MASK, estado, 0, 0 );
};

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
void mcu6050_i2c_bus_init(void)
{
	twihs_options_t bno055_option;
	pmc_enable_periph_clk(TWIHS_MCU6050_ID);

	/* Configure the options of TWI driver */
	bno055_option.master_clk = sysclk_get_cpu_hz();
	bno055_option.speed      = 40000;
	twihs_master_init(TWIHS_MCU6050, &bno055_option);
}

/*	\Brief: The function is used as I2C bus write
 *	\Return : Status of the I2C write
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be written
 *	\param reg_data : It is a value hold in the array,
 *		will be used for write the value into the register
 *	\param cnt : The no of byte of data to be write
 */
int8_t mcu6050_i2c_bus_write(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	int32_t ierror = 0x00;

	twihs_packet_t p_packet;
	p_packet.chip         = dev_addr;
	p_packet.addr[0]      = reg_addr;
	p_packet.addr_length  = 1;
	p_packet.buffer       = reg_data;
	p_packet.length       = cnt;
	
	ierror = twihs_master_write(TWIHS_MCU6050, &p_packet);

	return (int8_t)ierror;
}


 /*	\Brief: The function is used as I2C bus read
 *	\Return : Status of the I2C read
 *	\param dev_addr : The device address of the sensor
 *	\param reg_addr : Address of the first register, will data is going to be read
 *	\param reg_data : This data read from the sensor, which is hold in an array
 *	\param cnt : The no of byte of data to be read
 */
int8_t mcu6050_i2c_bus_read(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	int32_t ierror = 0x00;
	
	twihs_packet_t p_packet;
	p_packet.chip         = dev_addr;
	p_packet.addr[0]      = reg_addr;
	p_packet.addr_length  = 1;
	p_packet.buffer       = reg_data;
	p_packet.length       = cnt;
	
  // TODO: Algum problema no SPI faz com que devemos ler duas vezes o registrador para
  //       conseguirmos pegar o valor correto.
	ierror = twihs_master_read(TWIHS_MCU6050, &p_packet);
	ierror = twihs_master_read(TWIHS_MCU6050, &p_packet);

	return (int8_t)ierror;
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
   
  /* Disable the watchdog */
  WDT->WDT_MR = WDT_MR_WDDIS;
  
  /* Inicializa com serial com PC*/
  configure_console();
  
  /* Configura Leds */
  LED_init(1);
  
  /* Configura os botões */
  BUT_init();  
  
  /* Inicializa funcao de delay */
  delay_init( sysclk_get_cpu_hz());
   
  
  /************************************************************************/
  /* MPU                                                                  */
  /************************************************************************/
  
  /* Inicializa i2c */
  printf("Inicializando bus i2c \n");
  mcu6050_i2c_bus_init();
  
  // Verifica MPU
  rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_WHO_AM_I, bufferRX, 1);
  if(rtn != TWIHS_SUCCESS){
    printf("[ERRO] [i2c] [read] \n");
  }
  
  // Por algum motivo a primeira leitura é errada.
  if(bufferRX[0] != 0x68){
    printf("[ERRO] [mcu] [Wrong device] [0x%2X] \n", bufferRX[0]);
  }
 
  // Set Clock source
  bufferTX[0] = MPU6050_CLOCK_PLL_XGYRO;
  rtn = mcu6050_i2c_bus_write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_PWR_MGMT_1, bufferTX, 1);
  if(rtn != TWIHS_SUCCESS)
    printf("[ERRO] [i2c] [write] \n");

  // Configura range acelerometro para operar com 2G
  bufferTX[0] = 0x00; // 2G
  rtn = mcu6050_i2c_bus_write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_CONFIG, bufferTX, 1);
  rangePerDigit = 0.000061f ; // 2G       
 
	while (1) {
    // Le valor do acc X High e Low
    rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &accXHigh, 1);
    rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_L, &accXLow,  1);
   
    // Le valor do acc y High e  Low
    rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_YOUT_H, &accYHigh, 1);
    rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_ZOUT_L, &accYLow,  1);
    
    // Le valor do acc z HIGH e Low
    rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_ZOUT_H, &accZHigh, 1);
    rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_ZOUT_L, &accZLow,  1);
     
    // Dados são do tipo complemento de dois
    accX = (accXHigh << 8) | (accXLow << 0);
    accY = (accYHigh << 8) | (accYLow << 0);
    accZ = (accZHigh << 8) | (accZLow << 0);
   
    printf("x/y/z : %d / %d / %d \n", accX, accY, accZ);
   
    pin_toggle(LED_PIO, LED_PIN_MASK);
    delay_ms(300);
	}
}
