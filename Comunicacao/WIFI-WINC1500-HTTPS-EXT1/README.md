# WIFI (WINC1500) - HTTPS - EXT1

Este exemplo é exatamente o mesmo disponível no Atmel Studio para o SAM D21, no entanto portado para utilizar o SAME70 no lugar. Ele faz uma requisição GET via HTTPS (porta 443) a um servidor Web configurado no `main.h` (exatamente conforme os exemplos de HTTP).

Na pasta `Teste_Servidor`, existe um script em Python, server.py que irá subir um servidor HTTPS com certificado já gerado e configurado, pronto para testar com o Atmel.

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

### Erro socket: connect -12 error

Se você estiver obtendo este erro, é porque o seu módulo Wi-Fi WINC1500 não possui os certificados associados ao servidor e portanto não consegue estabelecer a autenticidade do servidor.
Para isto é preciso realizar um procedimento de atualização de firmware + carregamento de certificados, pedimos que procure o Marcos (técnico) ou o Eduardo (prof. Auxiliar) para auxiliar com o procedimento.

## Adaptando um outro projeto para usar HTTPS

### firmware

Não há grandes alterações no código do lado do firmware, caso tenha você já tenha um projeto usando HTTP com base nos outros exemplos, você pode converter o seu projeto (do lado do Atmel) em 2 simples passos:

1. Alterar a porta de conexão de 80 ou 5000 para 443 (HTTPS).

2. Adicionar um flag na criação do Socket para indicar que está criando um Socket que conectará usando SSL.

Basta trocar a linha:

```
tcp_client_socket = socket(AF_INET, SOCK_STREAM, 0)
```

por:

```
tcp_client_socket = socket(AF_INET, SOCK_STREAM, SOCKET_FLAGS_SSL)
```

E tudo pronto! (Do lado do firmware).

### Servidor

No lado do Servidor, o jeito mais fácil é aproveitar os certificados já existentes na pasta Teste_Server.

Você pode utilizar qualquer tecnologia de servidor Web, a maioria deles suportam os certificados de SSL.

## Criando seus próprios certificados

Por ser extremamente avançado, para os mais aventureiros, deixo estas referências que podem te auxiliar:

- [SSL Client Authentication Step By Step - Makethenmakeinstall.com](https://www.makethenmakeinstall.com/2014/05/ssl-client-authentication-step-by-step/)

- Guia de ferramentas da Atmel - Disponível no Atmel Studio ao criar projeto de Update do WINC1500
