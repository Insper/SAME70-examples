# PIO - IRQ

Exemplo que configura o pino do botão para gerar interrupção no microcontrolador. 

- Módulos: 
    - .
    
- Periféricos:
    - PIOC e PIOA
    
- Pinos:
    - `PC8`: LED
    - `PA11`: Botão
    - `stdio` / DEBUG
        - `PB4`:  UART1 
        - `PD21`: UART1
 
- APIs:
    - .

## Conexão e configuração

- Não é necessário


# Explicação

A seguir iremos ter uma breve explicação de como configurarmos uma interrupção para o PIO do SAME70, na verdade, iremos usar um driver que abstrai a parte mais baixa da interrupção e iremos conseguir configurar um [`callback`](https://en.wikipedia.org/wiki/Callback_(computer_programming)) para cada pino do PIO.

![](https://raw.githubusercontent.com/wiki/Insper/ComputacaoEmbarcada/imgs/PIO-IRQ/callback.png)

## Definindo a função de callBack

Devemos definir uma função de callBack que será chamada sempre que acontecer uma mudança no botão. 

```c
void but_callback(void){
  for (int i=0;i<5;i++)
  {
    pio_clear(LED_PIO, LED_IDX_MASK);
    delay_ms(200);
    pio_set(LED_PIO, LED_IDX_MASK);
    delay_ms(200);
  }
}
```

Essa função será chamada sempre que ocorrer uma interrupção em um pino, que será configurado para gerar um interrupção.

### Callback vs Handler

Utilizamos o termo `callback` para definir uma chamada de função baseada em evento realizado por um **software** e utilizamos o termo `handler` ou `interrupção` para uma chamada de função realizada pelo próprio **hardware**.

![](https://raw.githubusercontent.com/wiki/Insper/ComputacaoEmbarcada/imgs/PIO-IRQ/callback2.png)

## Configurando o PIO

Para configurarmos o PIO para gerar interrupção é necessário configurarmos duas parte distintas do uC. A primeira é o próprio PIO e a segunda é o `NVIC` (parte do CORE ARM responsável por receber e gerenciar interrupções).

A Atmel disponibiliza a função `pio_enable_interrupt()` que ativa e configura a interrupção do PIO em uma determinada combinações de pinos.

```c
 // Enable the given interrupt source.
 // The PIO must be configured as an NVIC interrupt source as well.
 // p_pio Pointer to a PIO instance.
 // ul_mask Interrupt sources bit map.
 void pio_enable_interrupt(Pio *p_pio, const uint32_t ul_mask);
```

O PIO gera somente uma interrupção: independente de qual pino do PIO foi ativado o código irá para a função `void PIO_Handler(void)` em questão. Porém a ASF fornece uma camada de abstração ([`pio_handler.c`](https://github.com/Insper/SAME70-examples/blob/master/Perifericos-uC/PIO-IRQ/PIO/src/ASF/sam/drivers/pio/pio_handler.c)) que possibilita que uma função seja atribuída por pino, dando a sensação que existe uma interrupção por pino quando na verdade isso é tratamento de software.

Uma vez ativada a interrupção em um determinado PIO, será necessário configurar o tipo de sinal que dará origem a essa interrupção, esses atributos estao definidos no arquivo [`/src/ASF/sam/drivers/pio.h`](https://github.com/Insper/SAME70-examples/blob/master/Perifericos-uC/PIO-IRQ/PIO/src/ASF/sam/drivers/pio/pio.h):

``` c
/*  Low level interrupt is active */
#define PIO_IT_LOW_LEVEL        (0               | 0 | PIO_IT_AIME)
/*  High level interrupt is active */
#define PIO_IT_HIGH_LEVEL       (PIO_IT_RE_OR_HL | 0 | PIO_IT_AIME)
/*  Falling edge interrupt is active */
#define PIO_IT_FALL_EDGE        (0               | PIO_IT_EDGE | PIO_IT_AIME)
/*  Rising edge interrupt is active */
#define PIO_IT_RISE_EDGE (PIO_IT_RE_OR_HL | PIO_IT_EDGE | PIO_IT_AIME)
```

Além do PIO, pino e atributo, a função `pio_handler_set()` recebe como parâmetro um ponteiro de função que será chamado sempre que a interrupção em um determinado **pino** ocorrer. 

> Note que podemos concaternar mais de um atributo por pino, possibilitando que ele gere uma interrupção por exemplo tanto em borda de descida quanto em borda de subida. Além do PIO, pino e atributo, a função [`pio_handler_set`](https://github.com/Insper/SAME70-examples/blob/master/Perifericos-uC/PIO-IRQ/PIO/src/ASF/sam/drivers/pio/pio_handler.c#L142) recebe como parâmetro um ponteiro de função que será chamado sempre que a interrupção em um determinado **pino** ocorrer. 

> Ponteiro de função? É pegar o endereço que a função está salva na ROM, para fazer isso basta passar o nome da função sem nenhum argumento.

```c
 // Set an interrupt handler for the provided pins.
 // The provided handler will be called with the triggering pin as its parameter
 //as soon as an interrupt is detected.
 // p_pio PIO controller base address.
 // ul_id PIO ID.
 // ul_mask Pins (bit mask) to configure.
 // ul_attr Pins attribute to configure.
 // p_handler Interrupt handler function pointer.
 // return 0 if successful, 1 if the maximum 
 // number of sources has been defined.
 int32_t pio_handler_set(Pio *p_pio, 
                         uint32_t ul_id, 
                         uint32_t ul_mask,
                         uint32_t ul_attr, 
                         void (*p_handler) (uint32_t,uint32_t));
```

Exemplo de uso:

```c
pio_handler_set(BUT_PIO,
                BUT_PIO_ID,
                BUT_IDX_MASK,
                PIO_IT_FALL_EDGE,
                but_callback);
```

### NVIC

O [Nested Vectored Interrupt Controller (NVIC)](http://infocenter.arm.com/help/topic/com.arm.doc.dai0179b/ar01s01s01.html) é a parte do núcleo ARM que lida com interrupções, nele podemos configurar se uma interrupção está ativa ou não, sua prioridade via duas funções definidas no [`CMSIS`](https://www.keil.com/pack/doc/CMSIS/Core/html/group__NVIC__gr.html):

``` c
NVIC_EnableIRQ (IRQn_Type IRQn)
NVIC_SetPriority (IRQn_Type IRQn, uint32_t priority)
```

Nessas funções o **IRQn** é o ID do periférico que está sendo configurado e o **priority** é a prioridade que cada periférico terá nas interrupções suportando até 255 diferentes níveis (depende do uC) sendo: 0 o valor para a maior prioridade e 255 de menor prioridade.

### Referências

- https://www.youtube.com/watch?v=uFBNf7F3l60
