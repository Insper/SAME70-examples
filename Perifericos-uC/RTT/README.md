# RTT - Real Timer Timer

O **Real Time Time** (RTT) é um timer similar ao TC, mas bem mais simples. Serve apenas para contar `clocks`. Possui um registrador de 32 bits, ao contrário do TC que o registrador é de 12 bits. O RTT pode ser alimentado por dois clocks distintos. O que permite gerarmos interrupções com grandes períodos de tempo.

- Módulos: 
    - OLED
    
- Periféricos:
    - PWM0 - Pulse Width Modulation
    - USART1 (debug - para comunicação com o PC - `stdio` )
    
- Pinos:
    - `PIOA`: LED do OLED
    - `PA11`: Botão
    - `stdio` / DEBUG
        - `PB4`:  UART1 
        - `PD21`: UART1
 
- APIs:
    - .

## Conexão e configuração

- Não é necessário

## Explicação

O RTT pode gerar interrupção por duas fontes distintas: `Alarme` ou `Mudança no valor do relógio`. O Alarme funciona similar ao alarme do RTC, podemos especificar um valor e quando o contador chegar nesse valor, gera uma interrupção, ou podemos gerar uma interrupção toda vez que o valor do timer mudar (tick). Isso vai depender da frequência na qual ele foi configurado (funcionando similar ao TC).

![](rtt.png)

