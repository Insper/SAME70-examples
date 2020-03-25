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
 
- APIs:
    - RTT

## Conexão e configuração

- Não é necessário

## Explicação

O RTT pode gerar interrupção por duas fontes distintas: `Alarme` ou `Mudança no valor do relógio`. O Alarme funciona similar ao alarme do RTC, podemos especificar um valor e quando o contador chegar nesse valor, gera uma interrupção, ou podemos gerar uma interrupção toda vez que o valor do timer mudar (tick). Isso vai depender da frequência na qual ele foi configurado (funcionando similar ao TC).

![](doc/rtt.png)


### `pllPreScale`

``` c
 uint16_t pllPreScale = (int) (((float) 32768) / 4.0);
```

O clock base do RTT é 32678Hz Para gerar outra base de tempo é necessário usar o PLL pre scale, que divide o clock base.

Nesse exemplo, estamos operando com um clock base de `pllPreScale = 32768/32768/4 = 2Hz`

Quanto maior a frequência maior a resolução, porém menor o tempo máximo que conseguimos contar.
    
Podemos configurar uma IRQ para acontecer quando o contador do RTT atingir um determinado valor aqui usamos o `irqRTTvalue` para isso.

!!! note
    Para gerar uma IRQ periódica precisamos reiniciar o RTT toda vez que ele atingiu o valor do alarme.
       
### `irqRTTvalue`

``` c
 uint32_t irqRTTvalue = 8;
```

Nesse exemplo o `irqRTTvalue = 8`, causando uma interrupção a cada 4 segundos (lembre que usamos o      pllPreScale, cada incremento do RTT leva 500ms (2Hz).  

### Periódo da interrupcao

O período da interrupcao será 4s, devido a conta a seguir:

```
    | pllPreScale
    v
 8*0.5 = 4s
 ^
 | irqRTTvalue
```    