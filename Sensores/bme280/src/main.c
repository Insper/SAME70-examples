/**
 *  Computação Embarcada
 *  Insper
 *  Rafael Corsi - rafael.corsi@insper.edu.br
 *  Código exemplo uso I2C
 *  Maio - 2019
 *
 *  Bug conhecido : spi read deve ser executado duas vezes ?
 *      - funcao afetada : mcu6050_i2c_bus_read()
 *
 *  Conectar :
 *            MCU  |  SAME70-XPLD | 
 *           ----------------------
 *            SDA  |   EXT2-11    |  PA3
 *            SCL  |   EXT2-12    |  PA4
 *            GND  |   EXT2-19    |
 *            VCC  |   EXT2-20    | 
 * 
 * TODO: calibracao
 *       bug dois reads i2c
 */

 
#include "asf.h"
#include "bme280.h"

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


/************************************************************************/
/* BME                                                                  */
/************************************************************************/ 
/*	
 *  \Brief: The function is used as I2C bus init
 */
void bme280_i2c_bus_init(void)
{
	twihs_options_t bno055_option;
	pmc_enable_periph_clk(TWIHS_MCU6050_ID);

	/* Configure the options of TWI driver */
	bno055_option.master_clk = sysclk_get_cpu_hz();
	bno055_option.speed      = 10000;
	twihs_master_init(TWIHS_MCU6050, &bno055_option);
}

uint8_t bme280_i2c_read_reg(uint CHIP_ADDRESS, uint reg_address, char *value){
  	uint i = 1;
    
  	  twihs_packet_t p_packet;
  	  p_packet.chip         = CHIP_ADDRESS;//BME280_ADDRESS;
  	  p_packet.addr_length  = 0;

  	  char data = reg_address; //BME280_CHIP_ID_REG;
  	  p_packet.buffer       = &data;
  	  p_packet.length       = 1;
  	
  	  if(twihs_master_write(TWIHS_MCU6050, &p_packet) != TWIHS_SUCCESS)
  	    return 1;

  	  p_packet.addr_length  = 0;
  	  p_packet.length       = 1;
      p_packet.buffer       = value;

  	  if(twihs_master_read(TWIHS_MCU6050, &p_packet) != TWIHS_SUCCESS)
  	    return 1;
        
    return 0;  
}

int8_t bme280_i2c_config_temp(void){
  	int32_t ierror = 0x00;
  	
  	twihs_packet_t p_packet;
  	p_packet.chip         = BME280_ADDRESS;//BME280_ADDRESS;
    p_packet.addr[0]      = BME280_CTRL_MEAS_REG;
  	p_packet.addr_length  = 1;

  	char data = 0b00100111; //BME280_CHIP_ID_REG;
  	p_packet.buffer       = &data;
  	p_packet.length       = 1;
  	
  	if(twihs_master_write(TWIHS_MCU6050, &p_packet) != TWIHS_SUCCESS)
  	return 1;
}

int8_t bme280_i2c_read_temp(uint *temp)
{
	int32_t ierror = 0x00;
  char tmp[3];
  
  bme280_i2c_read_reg(BME280_ADDRESS, BME280_TEMPERATURE_MSB_REG, &tmp[2]);
  bme280_i2c_read_reg(BME280_ADDRESS, BME280_TEMPERATURE_MSB_REG, &tmp[2]);
  
  bme280_i2c_read_reg(BME280_ADDRESS, BME280_TEMPERATURE_LSB_REG, &tmp[1]);
  bme280_i2c_read_reg(BME280_ADDRESS, BME280_TEMPERATURE_LSB_REG, &tmp[1]);

  *temp = tmp[2] << 8 | tmp[1];
	return 0;
}

uint8_t bme280_validate_id(void){
  char id;
  bme280_i2c_read_reg(BME280_ADDRESS, BME280_CHIP_ID_REG, &id );
  if (bme280_i2c_read_reg(BME280_ADDRESS, BME280_CHIP_ID_REG, &id )) 
    return 1;
  if (id != 0x60)
    return 1;
  return 0; 
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
  printf("Demo do sensor BME280, sem calibracao! \n");
  
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
  bme280_i2c_bus_init();
  delay_ms(10);
  
  /* verificando presenca do chip */
  while(bme280_validate_id()){
    printf("Chip nao encontrado\n");
    delay_ms(200);
  }    
   
  printf("Chip encontrado, inicializando temperatura \n");
  bme280_i2c_config_temp();
    
  uint temp;
	while (1) {
   if (bme280_i2c_read_temp(&temp))
      printf("erro readinG temperature \n");
   else
      printf("Temperatura: %d \n", temp);
      
   pin_toggle(LED_PIO, LED_PIN_MASK);
   delay_ms(1000);
	}
}
