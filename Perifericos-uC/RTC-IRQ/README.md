# RTC - IRQ

Configura o Real-time Clock - RTC para gerar uma interrupção após 20 segundos de operacao.

- Módulos: 
    - .
    
- Periféricos:
    - RTC - Real Time Timer
    - PIO

- Pinos:
    - `PC8`: LED

- APIs:
    - RTC

## Conexão e configuração

- Não é necessário

## Explicação

O RTC é um periférico do uC que serve para contar tempo com resolução de segundos, ele possui toda a lógica interna de um relógio, contando anos, meses, dias, horas, minutos e segundos (para ai!). 

No main, chamamos a funcao `RTC_init`, nela, temos que passar a configuracao inicial do calendario e relógio do RTC: `calendar rtc_initial = {2018, 3, 19, 12, 15, 45 ,1};`:

``` c
typedef struct  {
  uint32_t year;
  uint32_t month;
  uint32_t day;
  uint32_t week;
  uint32_t hour;
  uint32_t minute;
  uint32_t seccond;
} calendar;
```

``` c
/** Configura RTC */
calendar rtc_initial = {2018, 3, 19, 12, 15, 45 ,1};
RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_ALREN);
``` 

O RTC é configurado na função `RTC_init()`:

``` c
void RTC_init(Rtc *rtc, uint32_t id_rtc, calendar t, uint32_t irq_type){
	/* Configura o PMC */
	pmc_enable_periph_clk(ID_RTC);

	/* Default RTC configuration, 24-hour mode */
	rtc_set_hour_mode(rtc, 0);

	/* Configura data e hora manualmente */
	rtc_set_date(rtc, t.year, t.month, t.day, t.week);
	rtc_set_time(rtc, t.hour, t.minute, t.seccond);

	/* Configure RTC interrupts */
	NVIC_DisableIRQ(id_rtc);
	NVIC_ClearPendingIRQ(id_rtc);
	NVIC_SetPriority(id_rtc, 0);
	NVIC_EnableIRQ(id_rtc);

	/* Ativa interrupcao via alarme */
	rtc_enable_interrupt(rtc,  irq_type);
}
```
 
#### Alarme

O alarme é uma maneira de configurarmos o RTC para gerar uma interrupção em uma determinada data, no nosso caso, para um minuto após a inicialização do microcontrolador. 

```C
  main(){
  ...
  
  /* configura alarme do RTC */    
  rtc_set_date_alarm(RTC, 1, MOUNTH, 1, DAY);
  rtc_set_time_alarm(RTC, 1, HOUR, 1, MINUTE+1, 1, SECOND);
  
  }
```

#### Interrupção

Sempre que a situação do alarme for satisfeita, a função `RTC_Handler` será chamada. Na interrupção configura a variável global `flag_led0` para 0.

```C
/**
 * \brief Interrupt handler for the RTC. 
 */
void RTC_Handler(void)
{
  /* Qual parte do RTC gerou interrupção ? */
  /* IRQ */
  uint32_t ul_status = rtc_get_status(RTC);
  
  /* seta led para parar de piscar */
  flag_led0 = 0;
  
  /* Informa que interrupção foi tratada */
  rtc_clear_status(RTC, RTC_SCCR_ALRCLR);
}
```

Note que ao chamarmos a função `rtc_get_status(RTC)` estamos automaticamente realizando o `ACK` da interrupção.
