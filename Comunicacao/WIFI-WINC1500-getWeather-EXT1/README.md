# WIFI (WINC1500) - get weather - EXT1

Esse exemplo mostra como buwscar informações de clima usando uma conexão wifi. A rede wifi que o kit irá se conectar é obtida via módo: 'provisionado'.

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
 
:exclamation: Pinos em conflito com EXT2
 
## Conexão e configuração

> Conectar o WINC1500 no `EXT1` do kit de desenvolvimento. É necessário ter uma rede wifi disponível.

1. Embarque o exemplo no microcontrolador
1. Abra o terminal do AtmelStudio
1. O firmware deve criar uma rede wifi chamada: `ComputacaoEmbarcada`
1. Você deve a partir de outro dispositivo se conectar na rede criada
1. Acessar a página `wifiConfig.com`
1. Inserir os dados da rede WIFI na qual o uC vai se conectar
1. Analise o log do microcontrolador, ele deve exibir a temperatura em Sorocaba

### Configurando

Editar o arquivo `main.h`

## Resultados esperados

O exemplo deve criar um wifi em modo `provision`, e criar uma página web que permite a um usuário inserir informações da rede WIFI na qual o kit irá se conectar (para usar a internet), e então deve baixar as informações do clima de `Sorocaba`
