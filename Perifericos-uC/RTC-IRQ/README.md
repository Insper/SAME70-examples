# RTC - IRQ

Configura o Real-time Clock - RTC para gerar uma interrupção após 20 segundos de operacao.

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

Com a definição do calendário padrão podemos inicializar o RTC:

``` c
/** Configura RTC */
calendar rtc_initial = {2018, 3, 19, 12, 15, 45 ,1};
RTC_init(RTC, ID_RTC, rtc_initial, RTC_IER_ALREN);
``` 

Notem que o último argumento da função `RTC_init` define qual tipo de interrupção que o RTC irá gerar, temos algumas opções:

- `RTC_IER_ALREN`: IRQ quando o RTC atingir o alarme configurado
- `RTC_IER_SECEN`: IRQ a cada mudança de segundo

O RTC fornece duas funções `rtc_get_time` e `rtc_get_date` para leitura da hora e do calendário atual:

```c
uint32_t current_hour, current_min, current_sec;
uint32_t current_year, current_month, current_day; current_week;
rtc_get_time(RTC, &current_hour, &current_min, &current_sec);
rtc_get_date(RTC, &current_year, &current_month, &current_day, &current_week);
```

No exemplo estamos usando a IRQ por alarme, que e configurado para 20s após o ínicio do programa:

```c
/* configura alarme do RTC para daqui 20 segundos */                                                                  rtc_set_date_alarm(RTC, 1, current_hour.month, 1, current_hour.day);                              
rtc_set_time_alarm(RTC, 1, current_hour.hour, 1, current_hour.minute, 1, current_hour.second + 20);
//                                                                              ^- segundo atual + 20s
```

### Interrupção

Sempre que houver um IRQ a função `RTC_Handler` será chamada. Reparem que precisamos verificar o porque entramos na interrupção do IRQ, que pode ser por várias razões: Alarme, mudança de segunda... e então fazemos o processamento para cada um das possibilidades.

No exemplo interrupção configura a variável global `flag_rtc_alarm` para 1, indicando ao main que o IRQ de alarme aconteceu.

```C
/**
 * \brief Interrupt handler for the RTC. 
 */
void RTC_Handler(void) {
    uint32_t ul_status = rtc_get_status(RTC);
	
    /* seccond tick */
    if ((ul_status & RTC_SR_SEC) == RTC_SR_SEC) {	
	// o código para irq de segundo vem aqui
    }
	
    /* Time or date alarm */
    if ((ul_status & RTC_SR_ALARM) == RTC_SR_ALARM) {
    	// o código para irq de alame vem aqui
        flag_rtc_alarm = 1;
    }
    
    /// ...
}
```

### while(1)

No while(1) aguardamos a IRQ acontecer (que demora 20s após a placa ter sido ligada) e então piscamos o LED por 5 segundos.

```c
    while (1) {                                                                                     
      if(flag_rtc_alarm){                                                                                 
          pisca_led(5, 200);                                                                       
          flag_rtc_alarm = 0;                                                                             
       }                                                                                            
    }                                                                                               
```
