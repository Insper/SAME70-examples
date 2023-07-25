# WIFI (WINC1500) - TCP Socket Host - EXT1

Esse exemplo, demonstra como desenvolver um firmware que é capaz de abrir um socket TCP/IP, transmitir e receber dados por ele. O exemplo permite que o LED da placa seja controlado pelo PC.

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

Esse projeto possui dois softwares:

- `WIFI-TCP-Socket-client-EXT1`: firmware que configura o uC
- `python-socket-Client`: Programa em python a ser executado no PC

Deve seguir os mesmos passos do projeto 

Esse projeto possui dois softwares:

- `socketClient.py`: Script  a ser acessado pelo PC que se conecta ao sistema embarcado
- `WIFI-get-example-EXT1`: Firmware do uC que utiliza o WINC1500 

Você deve:

1. Configurar o `main.h` do exemplo, editando:

- A configuração da rede:

```c
#define MAIN_WLAN_SSID    "TP-LINK_9AE8" /**< Destination SSID */
#define MAIN_WLAN_AUTH    M2M_WIFI_SEC_WPA_PSK /**< Security manner */
#define MAIN_WLAN_PSK     "11002871" /**< Password for Destination SSID */
```

2. Configure o script python, adicionando o IP do sistema embarcado:

```py
  s.connect(("192.168.0.106", 6666))
```

> Para descobrir o IP basta abrir o terminal do AtmelStudio e procurar no log do uC.

3. Executar o script python no seu PC
    - :exclamation: Seu pc deve estar na mesma rede na qual o embarcado vai conectar
4. Envie comandos e observe o resultado no kit

## Resultado esperado

Que o programa em python se conecte no uC, e que por ele seja possível controlar os LEDs do kit.

