# WIFI (WINC1500) - get RTOS - EXT1

Esse exemplo demonstra como desenvolver um firmware (com RTOS) que é capaz de realizar `gets` em um webserver. 

- Módulos: 
    - WINC1500 (módulo WIFI)
    
- Periféricos:
    - SPI
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
    - `stdio` / DEBUG
        - `PB4`:  UART1 
        - `PD21`: UART1
 
- APIs:
    - driver WINC1500
    - freeRTOS
 
:exclamation: Pinos em conflito com EXT2

## Conexão e configuração

Igual ao do exemplo sem RTOS (`WIFI-WINC1500-get-EXT1`).

## Explicação

Esse exemplo apenas encapsula o código do wifi em uma task: `task_wifi`, não sendo a melhor maneira de resolver o problema.

