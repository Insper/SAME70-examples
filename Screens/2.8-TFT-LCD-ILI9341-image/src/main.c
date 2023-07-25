/**
 * Rafael Corsi @ insper . edu . br
 * 2021
 * Exemplo de uso do LCD 2.8" da adafruit
 * com controlador ili9341 com o kit same70-xpld
 *
 * neste exemplo a imagem foi convertida via
 * o programa imageconverter.
 *
 * 1. Configura o LCD
 * 1. Desenha a imagem no LCD
 *
 * Para conexão elétrica acesse o README.md
 *
 **/
#include <asf.h>
#include "ili9341.h"

typedef struct {
   const uint16_t *data;
   uint16_t width;
   uint16_t height;
   uint8_t dataSize;
 } tImage;

#include "image.h"

/** Display background color when clearing the display */
#define BG_COLOR  ILI9341_COLOR(255, 0, 255)

static void configure_lcd_pins(void) {
    /**LCD pin configure on SPI*/
    pio_configure_pin(LCD_SPI_MISO_PIO, LCD_SPI_MISO_FLAGS);
    pio_configure_pin(LCD_SPI_MOSI_PIO, LCD_SPI_MOSI_FLAGS);
    pio_configure_pin(LCD_SPI_SPCK_PIO, LCD_SPI_SPCK_FLAGS);
    pio_configure_pin(LCD_SPI_NPCS_PIO, LCD_SPI_NPCS_FLAGS);
    pio_configure_pin(LCD_SPI_RESET_PIO, LCD_SPI_RESET_FLAGS);
    pio_configure_pin(LCD_SPI_CDS_PIO, LCD_SPI_CDS_FLAGS);
}

/**
 * \brief Main application
 */
int main(void) {
	board_init();
	sysclk_init();

	/* initialize ios and the display controller */
    configure_lcd_pins();
	ili9341_init();

    /* set x,y pointer on screen */
    /* set update range  */
 	ili9341_set_top_left_limit(0, 0);
    ili9341_set_bottom_right_limit(Image.width, Image.height);

    /* send pxs to screen */
    ili9341_copy_pixels_to_screen(Image.data, ILI9341_DEFAULT_WIDTH*ILI9341_DEFAULT_HEIGHT);

    while(1) { }

}
