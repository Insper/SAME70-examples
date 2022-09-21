# RTT - Real Timer Timer

O **Real Time Time** (RTT) é um timer similar ao TC, mas bem mais simples. Serve apenas para contar `clocks`. Possui um registrador de 32 bits, ao contrário do TC que o registrador é de 12 bits. O RTT pode ser alimentado por dois clocks distintos. O que permite gerarmos interrupções com grandes períodos de tempo.

## Explicação

O RTT pode gerar interrupção por duas fontes distintas: `Alarme` ou `Mudança no valor do relógio`. O Alarme funciona similar ao alarme do RTC, podemos especificar um valor e quando o contador chegar nesse valor, gera uma interrupção, ou podemos gerar uma interrupção toda vez que o valor do timer mudar (tick). Isso vai depender da frequência na qual ele foi configurado (funcionando similar ao TC).

![](doc/rtt.png)

## RTT_init

A função `RTT_init` configura e ativa o RTT:

```c
static void RTT_init(float freqPrescale, uint32_t IrqNPulses, uint32_t rttIRQSource)
```

Vamos dar uma olhada nos argumentos:

- `freqPrescale`: Define a frequência na qual o RTT irá incrementar o contador. Quanto maior a frequência maior a resolução, porém menor o tempo máximo que conseguimos contar ( o contador é de 32bits).  
- `IrqNPulses`: Define o valor do arlarme do RTT, causando uma interrupção quando o contador alcançar tal valor. 
- `rttIRQSource`: Define qual tipo de interrupção será ativada, possíveis argumentos:
    - `NULL`: Sem interrupção ativida (pode passar qualquer valor para o `IrqNPulses`)
    - `RTT_MR_ALMIEN` : Interrupção por alarme (IrqNPulses)
    - ~`RTT_MR_RTTINCIEN`: Interrupção por incremento (pllPreScale)~ (existe mas não vamos usar pois deu muito problema no semestre anterior)

### RTT_Handler

Função chamada pelo NVIC quando acontece alguma interrupção do RTT.

```c
void RTT_Handler(void) {
  uint32_t ul_status;

  /* Get RTT status - ACK */
  ul_status = rtt_get_status(RTT);

  /* IRQ due to Time has changed */
  if ((ul_status & RTT_SR_RTTINC) == RTT_SR_RTTINC) {
     rtt_flag = 1;   // BLINK Led
   }
}
```

### main

1. O código começa configurando o RTT para operar com alarme: 
    - Alarme em 16 incrementos que é equivalente a 4s (1/freqPrescale * IrqNPulses ==> 1/4 * 16 = 4s)  
    - IRQ: Alarme contador
    
```c
    RTT_init(4, 16, RTT_MR_ALMIEN);
```

2. Passado os 4s o `RTT_handler` é chamado e a `flag_rtt` vira True

```c
void RTT_Handler(void) {
  uint32_t ul_status;
  ul_status = rtt_get_status(RTT);

  /* IRQ due to Alarm */
  if ((ul_status & RTT_SR_ALMS) == RTT_SR_ALMS) {
      flag_rtt = 1;
   }  
}
```

3. E no main, fazemos o toggle do pino do led e inicialziamos o contador novamente (para ele gerar um novo alarme):

```c
  flag_rtt = 1; // força flag = 1 para inicializar alarme.
  while (1) {
   if(flag_rtt) {
	  pin_toggle(LED_PIO, LED_IDX_MASK);    // BLINK Led
	  RTT_init(4, 16, RTT_MR_ALMIEN);  // inicializa rtt com alarme
	  flag_rtt = 0;
    }   
  }  
```
