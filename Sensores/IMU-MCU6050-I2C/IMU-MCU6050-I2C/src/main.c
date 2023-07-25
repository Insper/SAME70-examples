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
 *            MCU    |    PIN    |    SAME70-XPLD
 *           ------------------------------------
 *            SDA    |    PA3    |    EXT2-11   
 *            SCL    |    PA4    |    EXT2-12    	
 *            GND    |    GND    |    EXT2-19    
 *            VCC    |    VCC    |    EXT2-20     
 * 
 * ref [1] http://playground.arduino.cc/Main/MPU-6050#short
 * ref [2] https://github.com/jarzebski/Arduino-MPU6050/blob/master/MPU6050.cpp
 */

 
#include "asf.h"
#include "mpu6050.h"
#include "Fusion/Fusion.h"

/************************************************************************/
/* VAR globais                                                          */
/************************************************************************/
int16_t  raw_acc_x, raw_acc_y, raw_acc_z;
volatile uint8_t  raw_acc_xHigh, raw_acc_yHigh, raw_acc_zHigh;
volatile uint8_t  raw_acc_xLow,  raw_acc_yLow,  raw_acc_zLow;
float proc_acc_x, proc_acc_y, proc_acc_z;

int16_t  raw_gyr_x, raw_gyr_y, raw_gyr_z;
volatile uint8_t  raw_gyr_xHigh, raw_gyr_yHigh, raw_gyr_zHigh;
volatile uint8_t  raw_gyr_xLow,  raw_gyr_yLow,  raw_gyr_zLow;
float proc_gyr_x, proc_gyr_y, proc_gyr_z;

/************************************************************************/
/* PROTOTYPES                                                           */
/************************************************************************/

/************************************************************************/
/* Handlers                                                             */
/************************************************************************/

/************************************************************************/
/* Funcoes                                                              */
/************************************************************************/

static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource) {

	uint16_t pllPreScale = (int) (((float) 32768) / freqPrescale);
	
	rtt_sel_source(RTT, false);
	rtt_init(RTT, pllPreScale);
	
	if (rttIRQSource & RTT_MR_ALMIEN) {
		uint32_t ul_previous_time;
		ul_previous_time = rtt_read_timer_value(RTT);
		while (ul_previous_time == rtt_read_timer_value(RTT));
		rtt_write_alarm_time(RTT, IrqNPulses+ul_previous_time);
	}

	/* config NVIC */
	NVIC_DisableIRQ(RTT_IRQn);
	NVIC_ClearPendingIRQ(RTT_IRQn);
	NVIC_SetPriority(RTT_IRQn, 4);
	NVIC_EnableIRQ(RTT_IRQn);

	/* Enable RTT interrupt */
	if (rttIRQSource & (RTT_MR_RTTINCIEN | RTT_MR_ALMIEN))
	rtt_enable_interrupt(RTT, rttIRQSource);
	else
	rtt_disable_interrupt(RTT, RTT_MR_RTTINCIEN | RTT_MR_ALMIEN);
	
}

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
  
	/* Inicializa funcao de delay */
	delay_init( sysclk_get_cpu_hz());

	/* Inicializa Função de fusão */
	FusionAhrs ahrs;
	FusionAhrsInitialise(&ahrs);
  
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

	RTT_init(1000, 0, 0);         
	int tick_old = 0;
	
	while (1) {
		// Configura range acelerometro para operar com 2G
		bufferTX[0] = 0x00; // 2G
		rtn = mcu6050_i2c_bus_write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_CONFIG, bufferTX, 1);
		  
		// Le valor do acc X High e Low
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, &raw_acc_xHigh, 1);
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_XOUT_L, &raw_acc_xLow,  1);
   
		// Le valor do acc y High e  Low
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_YOUT_H, &raw_acc_yHigh, 1);
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_ZOUT_L, &raw_acc_yLow,  1);
    
		// Le valor do acc z HIGH e Low
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_ZOUT_H, &raw_acc_zHigh, 1);
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_ACCEL_ZOUT_L, &raw_acc_zLow,  1);
     
		// Dados são do tipo complemento de dois
		raw_acc_x = (raw_acc_xHigh << 8) | (raw_acc_xLow << 0);
		raw_acc_y = (raw_acc_yHigh << 8) | (raw_acc_yLow << 0);
		raw_acc_z = (raw_acc_zHigh << 8) | (raw_acc_zLow << 0);
		proc_acc_x = (float)raw_acc_x/16384;
		proc_acc_y = (float)raw_acc_y/16384;
		proc_acc_z = (float)raw_acc_z/16384;
		
		// Configura range gyroscopio para operar com 250 °/s
		bufferTX[0] = 0x00; // 250 °/s
		rtn = mcu6050_i2c_bus_write(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_CONFIG, bufferTX, 1);
				
		// Le valor do gyr X High e Low
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_XOUT_H, &raw_gyr_xHigh, 1);
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_XOUT_L, &raw_gyr_xLow,  1);
				
		// Le valor do gyr y High e  Low
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_YOUT_H, &raw_gyr_yHigh, 1);
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_ZOUT_L, &raw_gyr_yLow,  1);
				
		// Le valor do gyr z HIGH e Low
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_ZOUT_H, &raw_gyr_zHigh, 1);
		rtn = mcu6050_i2c_bus_read(MPU6050_DEFAULT_ADDRESS, MPU6050_RA_GYRO_ZOUT_L, &raw_gyr_zLow,  1);
				
		// Dados são do tipo complemento de dois
		raw_gyr_x = (raw_gyr_xHigh << 8) | (raw_gyr_xLow << 0);
		raw_gyr_y = (raw_gyr_yHigh << 8) | (raw_gyr_yLow << 0);
		raw_gyr_z = (raw_gyr_zHigh << 8) | (raw_gyr_zLow << 0);
		proc_gyr_x = (float)raw_gyr_x/131;
		proc_gyr_y = (float)raw_gyr_y/131;
		proc_gyr_z = (float)raw_gyr_z/131;
		
		// replace this with actual gyroscope data in degrees/s
		const FusionVector gyroscope = {proc_gyr_x, proc_gyr_y, proc_gyr_z}; 
		// replace this with actual accelerometer data in g	
		const FusionVector accelerometer = {proc_acc_x, proc_acc_y, proc_acc_z}; 
   
		// calcula runtime do código acima para definir delta t do programa
		int tick = rtt_read_timer_value(RTT);
		float delta_time = (tick - tick_old)/1000.0;
		tick_old = tick;

		// aplica o algoritmo
		FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, delta_time);

		// dados em pitch roll e yaw
		const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));
		
		printf("%f: Roll %0.1f, Pitch %0.1f, Yaw %0.1f\n", delta_time, euler.angle.roll, euler.angle.pitch, euler.angle.yaw);
	}
}
