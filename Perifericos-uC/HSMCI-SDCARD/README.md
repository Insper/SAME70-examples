# HSMCI - High Speed Multimedia Card Interface - SDCARD

Esse exemplo utiza o periférico MMC

- Módulos: 
    - SDCARD
    
- Periféricos:
    - HSMCI - High Speed Multimedia Card Interface
    - XDMA - Direct Memmory Access 
    - USART1 (debug - para comunicação com o PC - `stdio` )
    
- Pinos:
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
    - [FatFS](http://elm-chan.org/fsw/ff/00index_e.html)

:exclamation: Pinos em conflito com EXT2

## Conexão e configuração

- Formatar SDCARD como fat32.
- Conectar SDCARD no kit quando indicano no terminal.

## Explicação

Esse código utiliza o periférico HSMCI para escrever e ler do cartão de memória, a parte refente a partição (criar/ abrir/ escrever em arquivos) é realizada pela API opensource [FatFs](http://elm-chan.org/fsw/ff/00index_e.html). 
