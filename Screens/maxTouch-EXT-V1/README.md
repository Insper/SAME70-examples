
* OutEXT-LCD-maXTouch
* Eduardo Marossi
* Marco Mello - marcoasma@insper.edu.br
* Rafael Corsi
* 06/06/2019

<p align="center">
  <img width="480" height="480" src="./imagem.jpeg">
</p>

# Funcionamento :
Esse exemplo faz o uso do maXTouch Xplained Pro via USART2 configurada como SPI, liberando o SPI0, permitindo por exemplo a utilização do maXTouch junto com o ATWINC1500-XPRO (WiFi).

# Periféricos uC:
- USART2 como SPI
   
# Módulos/Componentes:
- maXTouch Xplained Pro

# Montagem:
	maXTouch       SAME70
	
   	05 -- DCX	PA1
	07 -- PWM	PA5
	09 -- IRQ	PA26
	10 -- RS	PA29
	11 -- SDA	PA3
	12 -- SCL	PA4
	15 -- CS	PD18
	16 -- MOSI	PD16
	17 -- MISO	PD15
	18 -- SCK	PD17
	19 -- GND	GND
	20 -- VCC	3.3V

# Diagrama
