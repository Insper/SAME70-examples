
* Processamento de Sinais + Buffer Ping Pong (modulo externo)
* Sabrina Simão - sabrinass@al.insper.edu.br
* 30/04/2019

# Resumo :

Esse exemplo demonstra como interfacear o conversor analógico-digital (AFEC) com o conversor digital-analógico (DAC) para receber um sinal de input e criar um sinal de saída transformado

Periféricos uC:

- Analog to Digital Controller (DAC)
- Analog Front End Controller (AFEC)

   
Equipamentos : 

- Gerador de Sinais
- Osciloscópio

Pinos específicos :

1) DAC
- DAC0 -> PB13
- GND


2) AFEC
- AFEC0 -> PD30
- GND


## Diagrama

![](ProcSinais.png)

## Funcionamento

Um sinal analógico entra através dos cabos GND e PD30, onde é armazenado na memória interna via o módulo do Buffer Ping Pong. Este sinal pode ser alterado e processado, e depois é convertido novamente em sinal digital pelo DAC e sua saída é o pino PB13 com GND.

O Buffer Ping Pong é um módulo feito para evitar lag no output do sinal, e seu funcionamento segue o diagrama a seguir:

![](PingPong.png)

Assim, um bloco de sinais vai sendo armazenado in real time enquanto o já preenchido começa a ser processador pelo uC. Quando este bloco estiver pronto para sair e o outro estiver cheio, há uma troca entre buffers ativos, e o primeiro começa a ser preenchido enquanto o segundo é processado. Claro que o tamanho de tal buffer deve sempre ser estudado para cada aplicação. 

