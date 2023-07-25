/**
 * \file
 *
 * \brief SSD1306 OLED display controller driver.
 *
 * Copyright (c) 2012-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "ssd1306.h"

/**
 * \internal
 * \brief Initialize the hardware interface
 *
 * Depending on what interface used for interfacing the OLED controller this
 * function will initialize the necessary hardware.
 */

#define SPI_CS_MASK 25
#define SPI_MOSI_MASK 21
#define SPI_CLK_MASK 22


static void ssd1306_interface_init(void)
{
	
	ioport_set_pin_dir(SSD1306_RES_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(SSD1306_DC_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_mode(SSD1306_RES_PIN, IOPORT_MODE_PULLUP);
	ioport_set_pin_mode(SSD1306_DC_PIN, IOPORT_MODE_PULLUP);
	ioport_enable_pin(SSD1306_DC_PIN);
	ioport_enable_pin(SSD1306_RES_PIN);
	ioport_set_pin_level(SSD1306_DC_PIN, true);
	ioport_set_pin_level(SSD1306_RES_PIN, true);
	
	
	pio_configure(PIOD, PIO_PERIPH_B, 1 << SPI_CLK_MASK, PIO_DEFAULT);
	pio_configure(PIOD, PIO_PERIPH_B, 1 << SPI_MOSI_MASK, PIO_DEFAULT);
	pio_configure(PIOD, PIO_PERIPH_B, 1 << SPI_CS_MASK, PIO_DEFAULT);
	
		pio_configure(PIOD, PIO_PERIPH_B, 1 << SPI_CLK_MASK, PIO_DEFAULT);
		pio_configure(PIOD, PIO_PERIPH_B, 1 << SPI_MOSI_MASK, PIO_DEFAULT);
		pio_configure(PIOD, PIO_PERIPH_B, 1 << SPI_CS_MASK, PIO_DEFAULT);
		
		spi_disable(SPI0);
		spi_reset(SPI0);
		spi_set_master_mode(SPI0);
		//spi_set_transfer_delay(SPI0, 1, 40, 30);
		spi_configure_cs_behavior(SPI0, 1, SPI_CS_KEEP_LOW);
		spi_set_clock_polarity(SPI0, 1, 0);
		spi_set_clock_phase(SPI0, 1, 0);
		spi_set_fixed_peripheral_select(SPI0);
		spi_set_bits_per_transfer(SPI0, 1, SPI_CSR_BITS_8_BIT);
		spi_disable_loopback(SPI0);

		spi_disable_mode_fault_detect(SPI0);
		int div = spi_calc_baudrate_div(2000000, sysclk_get_peripheral_hz());
		spi_set_baudrate_div(SPI0,1, div);
		spi_enable_clock(SPI0);
		
		spi_enable(SPI0);
}

/**
 * \brief Initialize the OLED controller
 *
 * Call this function to initialize the hardware interface and the OLED
 * controller. When initialization is done the display is turned on and ready
 * to receive data.
 d
 a
 a
 a
 */
void ssd1306_init(void)
{
	// Initialize delay routine
	delay_init();

	// Initialize the interface
	ssd1306_interface_init();

	// Do a hard reset of the OLED display controller
	ssd1306_hard_reset();

	// Set the reset pin to the default state
	ioport_set_pin_level(SSD1306_RES_PIN, true);
	
	// 1/32 Duty (0x0F~0x3F)
	ssd1306_write_command(SSD1306_CMD_SET_MULTIPLEX_RATIO);
	ssd1306_write_command(0x1F);

	// Shift Mapping RAM Counter (0x00~0x3F)
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_OFFSET);
	ssd1306_write_command(0x00);

	// Set Mapping RAM Display Start Line (0x00~0x3F)
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_START_LINE(0x00));

	// Set Column Address 0 Mapped to SEG0
	ssd1306_write_command(SSD1306_CMD_SET_SEGMENT_RE_MAP_COL127_SEG0);

	// Set COM/Row Scan Scan from COM63 to 0
	ssd1306_write_command(SSD1306_CMD_SET_COM_OUTPUT_SCAN_DOWN);

	// Set COM Pins hardware configuration
	ssd1306_write_command(SSD1306_CMD_SET_COM_PINS);
	ssd1306_write_command(0x02);

	ssd1306_set_contrast(0x8F);

	// Disable Entire display On
	ssd1306_write_command(SSD1306_CMD_ENTIRE_DISPLAY_AND_GDDRAM_ON);

	ssd1306_display_invert_disable();

	// Set Display Clock Divide Ratio / Oscillator Frequency (Default => 0x80)
	ssd1306_write_command(SSD1306_CMD_SET_DISPLAY_CLOCK_DIVIDE_RATIO);
	ssd1306_write_command(0x80);

	// Enable charge pump regulator
	ssd1306_write_command(SSD1306_CMD_SET_CHARGE_PUMP_SETTING);
	ssd1306_write_command(0x14);

	// Set VCOMH Deselect Level
	ssd1306_write_command(SSD1306_CMD_SET_VCOMH_DESELECT_LEVEL);
	ssd1306_write_command(0x40); // Default => 0x20 (0.77*VCC)

	// Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	ssd1306_write_command(SSD1306_CMD_SET_PRE_CHARGE_PERIOD);
	ssd1306_write_command(0xF1);

	
	ssd1306_display_on();
}

/**
 * \brief Writes a command to the display controller
 *
 * This functions pull pin D/C# low before writing to the controller. Different
 * data write function is called based on the selected interface.
 *
 * \param command the command to write
 */
void ssd1306_write_command(uint8_t command)
{
	ioport_set_pin_level(SSD1306_DC_PIN, false);
	
	spi_set_peripheral_chip_select_value(SPI0, 1);
	spi_write(SPI0, command, 1, 1);
	delay_us(10);
}

/**
 * \brief Write data to the display controller
 *
 * This functions sets the pin D/C# before writing to the controller. Different
 * data write function is called based on the selected interface.
 *
 * \param data the data to write
 */
void ssd1306_write_data(uint8_t data)
{
	ioport_set_pin_level(SSD1306_DC_PIN, true);
	
	spi_set_peripheral_chip_select_value(SPI0, 1);
	spi_write(SPI0, data, 1, 1);
	delay_us(10);
}
