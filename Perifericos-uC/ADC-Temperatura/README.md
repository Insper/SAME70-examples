O código exemplo configura o conversor analógico digital (`AFEC`) do microcontrolador SAME-70, esse periférico pode ser utilizado para transformar valores analógicos do mundo externo ao processador em valores digitais que podem ser processados pelo uC.

Em sua grande maioria os sensores elétricos são transdutores, ou seja, convertem convertem grandezas diversas em tensão (sensor de luz, sensor de temperatura, gás, ...). Para podermos processar essas grandezas é necessário primeiramente digitalizarmos esses valores.

Periféricos utilizados :
 - Power Management Controller (PMC)
 - Analog Front-End Controller (AFEC)
 - Universal Synchronous Asynchronous Receiver Transceiver (USART)

## Código Visão geral

1. configura o AFEC0 `config_ADC_TEMP()`
1. configura a uart para servir como console `configure_console()`
1. inicializa uma conversão `afec_start_software_conversion(AFEC0)` 
1. No while (1)
    - verifica a flag que indica o fim da conversão `g_is_conversion_done`
        - configurada na interrupção/callback do ADC `AFEC_Temp_callback`
        - converte valor de bits em tensão e então em temperatura `convert_adc_to_temp`
        - envia pela serial o resultado `printf`
        - inicializa uma nova conversão `afec_start_software_conversion(AFEC0)` 
        - espera 1 segundo `delay_s(1)`

### AFEC

O Analog Front-End Controller (AFEC) nesse exemplo é configurado para operar lendo o sensor de temperatura interno do microcontrolador,
que está conectado na entrada 11 do MUX. 


### `static int32_t convert_adc_to_temp(int32_t ADC_value)`

Essa é uma função que converte o valor lido (ADC\_value) pelo conversor analógico digital e converte para temperatura
em graus célsius. Sua implementação são duas simples transformações :

1. Regra de três para converter o valor em bits para Volts
2. Equacionamento linear para transformar Volts em graus Celsius

## `static void AFEC_Temp_callback(void)`

É a função que é chamada quando a conversão analógica digital é concluida. Nessa função fazemos a leitura do valor da
conversão analógica e salvamos o seu valor em uma variável global.

```C
g_ul_value = afec_channel_get_value(AFEC0, AFEC_CHANNEL_TEMP_SENSOR);
```

CallBacks são interrupções já tratadas por software, no caso do AFEC não tratamos diretamente o HANDLER mas sim callbacks que são
configuradas para cada canal do AFEC0:

```C
main(){
  ...

  /* configura call back */
  afec_set_callback(AFEC0, AFEC_INTERRUPT_EOC_11,	AFEC_Temp_callback, 1);
  ...
}
```

![CallBack](figs/handler.png){width=60%}

Na função de inicialização do AFEC chamada no `main` fazemos a associação do canal 11 que o sensor de temperatura está conectado a função de callback `AFEC_Temp_callback` nesse caso não trabalhamos com o Handler direto, mas sim com chamadas de funções internas a ele. O Handler do AFEC é gerenciado pelas funções da Atmel ASF, assim como as interrupções dos pinos do PIO.


### printf ?

Notou que nesse exemplo possuímos `printf`? Isso é possível pois estamos usando uma serial do uC que está conectada ao gravado do chip. Essa serial é reservada para debug. O que fizemos foi inicializar essa serial (USAR1) e depois configuramos o `stdio` para enviar e receber chars desse periférico.

```c
  stdio_serial_init(CONF_UART, &uart_serial_options);
```

####  `configure_console(void)`

A função `configure_console()` inicializa o periférico **USART** para controlar os pinos (PA21, PB4) do microcontrolador,
esses pinos estão conectados no EDBG (gravador embarcado no kit) que possibilitam abrimos um terminal no computador
e recebermos/enviarmos dados via uma comunicação serial, similar como é feito no arduino.

A função configura também o **stdio** para utilizar esse periférico como saída e entrada das funções **printf**, **getc**, .... .

![edbg comunicação UART - XPLD](figs/edbg.png)

##### NÃO DEVE USAR PRINTF DENTRO DE HANDLER!

Muito ruim! bobajada! não pode! 
