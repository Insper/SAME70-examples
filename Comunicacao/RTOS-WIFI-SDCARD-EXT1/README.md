# WIFI (WINC1500) - SDcard -  get - RTOS - EXT1

Esse firmware é um exemplo de como usar o WINC1500 e o SDCARD com RTOS

- Módulos: 
    - WINC1500 (módulo WIFI)
    - SDCARD
    
- Periféricos:
    - SPI
    - HSMCI - High Speed Multimedia Card Interface
    - XDMA - Direct Memmory Access 
    - USART1 (debug - para comunicação com o PC - `stdio` )
    
- Pinos:
    - WINC1500
        - EXT1 5 - RESET_N  
        - EXT1 6 - WAKE
        - EXT1 9 - IRQ_N
        - EXT1 10 - CHIP_EN
        - EXT1 15 - SPI_SSN
        - EXT1 16 - SPI_MOSI
        - EXT1 17 - SPI_MISO
        - EXT1 18 - SPI_CLK
    - HSMCI: 
        - `PA30`: SD_D0
        - `PA31`: SD_D1 
        - `PA26`: SD_D2
        - `PA27`: SD_D3 
        - `PA25`: SD_CLK
        - `PA28`: SD_CMD 
        - `PC16`: SD_DETECT
    - `stdio` / DEBUG
        - `PB4`:  UART1 
        - `PD21`: UART1
 
- APIs:
    - driver WINC1500
    - [FatFS](http://elm-chan.org/fsw/ff/00index_e.html)
    - freeRTOS
 
:exclamation: Pinos em conflito com EXT2

## Conexão e configuração

- Igual ao do exemplo sem RTOS (`WIFI-WINC1500-get-EXT1`).
- Formatar SDCARD como fat32.
- Conectar SDCARD no kit quando indicano no terminal.

## Explicação


