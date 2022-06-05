/**
 *
 * -# Build the program and download it into the evaluation board.
 * -# On the computer, open and configure a terminal application
 *    (e.g., HyperTerminal on Microsoft Windows) with these settings:
 *   - 115200 bauds
 *   - 8 bits of data
 *   - No parity
 *   - 1 stop bit
 *   - No flow control
 * -# In the terminal window, the following text should appear (values depend
 *    on the board and chip used):
 *    \code
	-- DAC Sinewave Example xxx --
	-- xxxxxx-xx
	-- Compiled: xxx xx xxxx xx:xx:xx --
	-- Menu Choices for this example--
	-- 0: Set frequency(200Hz-3kHz).--
	-- 1: Set amplitude(100-4095).--
	-- i: Display present frequency and amplitude.--
	-- m: Display this menu.--
\endcode
 * -# Input command according to the menu.
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include "asf.h"
#include "conf_board.h"
#include "conf_clock.h"
#include "conf_dacc_sinewave_example.h"

/** Analog control value */
#if (SAMV70 || SAMV71 || SAME70 || SAMS70)
#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) | DACC_ACR_IBCTLCH1(0x02))
#else
#define DACC_ANALOG_CONTROL (DACC_ACR_IBCTLCH0(0x02) | DACC_ACR_IBCTLCH1(0x02) | DACC_ACR_IBCTLDACCORE(0x01))
#endif

/** The maximal sine wave sample data (no sign) */
#define MAX_DIGITAL (0x7ff)
/** The maximal (peak-peak) amplitude value */
#define MAX_AMPLITUDE (DACC_MAX_DATA)
/** The minimal (peak-peak) amplitude value */
#define MIN_AMPLITUDE (100)

/** SAMPLES per cycle */
#define SAMPLES (100)

/** Default frequency */
#define DEFAULT_FREQUENCY 1000
/** Min frequency */
#define MIN_FREQUENCY 200
/** Max frequency */
#define MAX_FREQUENCY 3000

/** Invalid value */
#define VAL_INVALID 0xFFFFFFFF

#define STRING_EOL "\r"
#define STRING_HEADER "-- DAC Sinewave Example --\r\n" \
					  "-- " BOARD_NAME " --\r\n"       \
					  "-- Compiled: "__DATE__          \
					  " "__TIME__                      \
					  " --" STRING_EOL

/*! Convert wave data to DACC value
 *  Put the sinewave to an offset of max_amplitude/2.
 *  \param wave          Waveform data
 *  \param amplitude     Amplitude value
 *  \param max_digital   Maximal digital value of input data (no sign)
 *  \param max_amplitude Maximal amplitude value
 */
#define wave_to_dacc(wave, amplitude, max_digital, max_amplitude) \
	(((int)(wave) * (amplitude) / (max_digital)) + (max_amplitude / 2))

/** Current g_ul_index_sample */
uint32_t g_ul_index_sample = 0;
/** Frequency */
uint32_t g_ul_frequency = 0;
/** Amplitude */
int32_t g_l_amplitude = 0;

/** Waveform selector */
uint8_t g_uc_wave_sel = 0;

/** 100 points of sinewave samples, amplitude is MAX_DIGITAL*2 */
const int16_t gc_us_sine_data[SAMPLES] = {
	0x0, 0x080, 0x100, 0x17f, 0x1fd, 0x278, 0x2f1, 0x367, 0x3da, 0x449,
	0x4b3, 0x519, 0x579, 0x5d4, 0x629, 0x678, 0x6c0, 0x702, 0x73c, 0x76f,
	0x79b, 0x7bf, 0x7db, 0x7ef, 0x7fb, 0x7ff, 0x7fb, 0x7ef, 0x7db, 0x7bf,
	0x79b, 0x76f, 0x73c, 0x702, 0x6c0, 0x678, 0x629, 0x5d4, 0x579, 0x519,
	0x4b3, 0x449, 0x3da, 0x367, 0x2f1, 0x278, 0x1fd, 0x17f, 0x100, 0x080,

	-0x0, -0x080, -0x100, -0x17f, -0x1fd, -0x278, -0x2f1, -0x367, -0x3da, -0x449,
	-0x4b3, -0x519, -0x579, -0x5d4, -0x629, -0x678, -0x6c0, -0x702, -0x73c, -0x76f,
	-0x79b, -0x7bf, -0x7db, -0x7ef, -0x7fb, -0x7ff, -0x7fb, -0x7ef, -0x7db, -0x7bf,
	-0x79b, -0x76f, -0x73c, -0x702, -0x6c0, -0x678, -0x629, -0x5d4, -0x579, -0x519,
	-0x4b3, -0x449, -0x3da, -0x367, -0x2f1, -0x278, -0x1fd, -0x17f, -0x100, -0x080};

/**
 * \brief Configure console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console UART. */
#if !SAM4L
	sysclk_enable_peripheral_clock(CONSOLE_UART_ID);
#endif
	stdio_serial_init((Usart *)CONF_UART, &uart_serial_options);
}

/**
 * \brief Get input from user, and the biggest 4-digit decimal number is allowed.
 *
 * \param ul_lower_limit The lower limit of input
 * \param ul_upper_limit The upper limit of input
 */
static uint32_t get_input_value(uint32_t ul_lower_limit,
								uint32_t ul_upper_limit)
{
	uint32_t i = 0, length = 0, value = 0;
	uint8_t uc_key, str_temp[5] = {0};

	while (1)
	{
		usart_serial_getchar((Usart *)CONSOLE_UART, &uc_key);

		if (uc_key == '\n' || uc_key == '\r')
		{
			puts("\r");
			break;
		}

		if ('0' <= uc_key && '9' >= uc_key)
		{
			printf("%c", uc_key);
			str_temp[i++] = uc_key;

			if (i >= 4)
			{
				break;
			}
		}
	}

	str_temp[i] = '\0';
	/* Input string length */
	length = i;
	value = 0;

	/* Convert string to integer */
	for (i = 0; i < 4; i++)
	{
		if (str_temp[i] != '0')
		{
			switch (length - i - 1)
			{
			case 0:
				value += (str_temp[i] - '0');
				break;

			case 1:
				value += (str_temp[i] - '0') * 10;
				break;

			case 2:
				value += (str_temp[i] - '0') * 100;
				break;

			case 3:
				value += (str_temp[i] - '0') * 1000;
				break;
			}
		}
	}

	if (value > ul_upper_limit || value < ul_lower_limit)
	{
		puts("\n\r-F- Input value is invalid!");
		return VAL_INVALID;
	}

	return value;
}

/**
 * \brief Display main menu.
 */
static void display_menu(void)
{
	puts("======== Menu Choices for this example ========\r");
	printf("-- 0: Set frequency(%dHz-%dkHz).\n\r",
		   MIN_FREQUENCY, MAX_FREQUENCY / 1000);
	printf("-- 1: Set amplitude(%d-%d).\n\r", MIN_AMPLITUDE, MAX_AMPLITUDE);
	puts("-- i: Display present frequency and amplitude.\n\r"
		 "-- w: Switch to full amplitude square wave or back.\n\r"
		 "-- m: Display this menu.\n\r"
		 "------------ Current configuration ------------\r");
	printf("-- DACC channel:\t%d\n\r", DACC_CHANNEL);
	printf("-- Amplitude   :\t%ld\n\r", (long)g_l_amplitude);
	printf("-- Frequency   :\t%lu\n\r", (unsigned long)g_ul_frequency);
	printf("-- Wave        :\t%s\n\r", g_uc_wave_sel ? "SQUARE" : "SINE");
	puts("===============================================\r");
}

/**
 * \brief SysTick IRQ handler.
 */
void SysTick_Handler(void)
{
	uint32_t status;
	uint32_t dac_val;

	status = dacc_get_interrupt_status(DACC_BASE);

	/* If ready for new data */
#if (SAMV70 || SAMV71 || SAME70 || SAMS70)
#if (DACC_CHANNEL == 0)
	if ((status & DACC_ISR_TXRDY0) == DACC_ISR_TXRDY0)
	{
#elif (DACC_CHANNEL == 1)
	if ((status & DACC_ISR_TXRDY1) == DACC_ISR_TXRDY1)
	{
#endif
#else
	if ((status & DACC_ISR_TXRDY) == DACC_ISR_TXRDY)
	{
#endif
		g_ul_index_sample++;
		if (g_ul_index_sample >= SAMPLES)
		{
			g_ul_index_sample = 0;
		}
		dac_val = g_uc_wave_sel ? ((g_ul_index_sample > SAMPLES / 2) ? 0 : MAX_AMPLITUDE)
								: wave_to_dacc(gc_us_sine_data[g_ul_index_sample],
											   g_l_amplitude,
											   MAX_DIGITAL * 2, MAX_AMPLITUDE);
#if !(SAMV70 || SAMV71 || SAME70 || SAMS70)
		dacc_write_conversion_data(DACC_BASE, dac_val);
#else
		dacc_write_conversion_data(DACC_BASE, dac_val, DACC_CHANNEL);
#endif
	}
}

void start_dacc(void)
{
	/* Enable clock for DACC */
	sysclk_enable_peripheral_clock(DACC_ID);

	/* Reset DACC registers */
	dacc_reset(DACC_BASE);

	/* Enable output channel DACC_CHANNEL */
	dacc_enable_channel(DACC_BASE, DACC_CHANNEL);

	/* Set up analog current */
	dacc_set_analog_control(DACC_BASE, DACC_ANALOG_CONTROL);
}

/**
 *  \brief DAC Sinewave application entry point.
 *
 *  \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	uint8_t uc_key;
	uint32_t ul_freq, ul_amp;

	/* Initialize the system */
	sysclk_init();
	board_init();

	/* Initialize debug console */
	configure_console();

	start_dacc();

	/* Output example information */
	puts(STRING_HEADER);

	g_l_amplitude = MAX_AMPLITUDE / 2;
	g_ul_frequency = DEFAULT_FREQUENCY;

	SysTick_Config(sysclk_get_cpu_hz() / (g_ul_frequency * SAMPLES));

	/* Main menu */
	display_menu();

	while (1)
	{
		usart_serial_getchar((Usart *)CONSOLE_UART, &uc_key);

		switch (uc_key)
		{
		case '0':
			printf("Frequency:\t");
			ul_freq = get_input_value(MIN_FREQUENCY, MAX_FREQUENCY);
			printf("\r\n");

			if (ul_freq != VAL_INVALID)
			{
				printf("Set frequency to : %luHz\n\r", (unsigned long)ul_freq);
				SysTick_Config(sysclk_get_cpu_hz() / (ul_freq * SAMPLES));
				g_ul_frequency = ul_freq;
			}
			break;

		case '1':
			printf("Amplitude:\t");
			ul_amp = get_input_value(MIN_AMPLITUDE, MAX_AMPLITUDE);
			printf("\r\n");
			if (ul_amp != VAL_INVALID)
			{
				printf("Set amplitude to : %lu\n\r", (unsigned long)ul_amp);
				g_l_amplitude = ul_amp;
			}
			break;

		case 'i':
		case 'I':
			printf("-I- Frequency : %lu Hz Amplitude : %ld\n\r",
				   (unsigned long)g_ul_frequency, (long)g_l_amplitude);
			break;

		case 'w':
		case 'W':
			printf("-I- Switch wave to : %s\n\r", g_uc_wave_sel ? "SINE" : "Full Amplitude SQUARE");
			g_uc_wave_sel = (g_uc_wave_sel + 1) & 1;
			break;

		case 'm':
		case 'M':
			display_menu();
			break;
		}
		puts("Press \'m\' or \'M\' to display the main menu again!\r");
	}
}
