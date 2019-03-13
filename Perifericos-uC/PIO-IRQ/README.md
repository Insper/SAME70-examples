A seguir iremos ter uma breve explicação de como configurarmos uma interrupção para o PIO do SAME70, na verdade, iremos usar um driver que abstrai a parte mais baixa da interrupção e iremos conseguir configurar um [callback](https://en.wikipedia.org/wiki/Callback_(computer_programming)) para cada pino do PIO.

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

O PIO gera somente uma interrupção: independente de qual pino do PIO foi ativado o código irá para a função `void PIO_Handler(void)` em questão. Porém a ASF fornece uma camada de abstração (`pio_handler.c`) que possibilita que uma função seja atribuída por pino, dando a sensação que existe uma interrupção por pino quando na verdade isso é tratamento de software.

Uma vez ativada a interrupção em um determinado periférico, será necessário configurar o tipo de sinal que dará origem a essa interrupção, esses atributos estao definidos no arquivo `/src/ASF/sam/drivers/pio.h`:

- Borda de descida (PIO_IT_FALL_EDGE)
- Borda de subida (PIO_IT_RISIG_EDGE)
- Nível alto (PIO_IT_HIGH_LEVEL)
- ...

> Note que podemos concaternar mais de um atributo por pino, possibilitando que ele gere uma interrupção por exemplo tanto em borda de descida quanto em borda de subida.

Além do PIO, pino e atributo, a função `pio_handler_set` recebe como parâmetro um ponteiro de função que será chamado sempre que a interrupção em um determinado **pino** ocorrer. 


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

## NVIC

O [Nested Vectored Interrupt Controller (NVIC)](http://infocenter.arm.com/help/topic/com.arm.doc.dai0179b/ar01s01s01.html) é a parte do núcleo ARM que lida com interrupções, nele podemos configurar se uma interrupção está ativa ou não, sua prioridade via duas funções definidas no CMSIS :

``` c
NVIC_EnableIRQ (IRQn_Type IRQn)
NVIC_SetPriority (IRQn_Type IRQn, uint32_t priority)
```

Nessas funções o **IRQn** é o ID do periférico que está sendo configurado e o **priority** é a prioridade que cada periférico terá nas interrupções suportando até 255 diferentes níveis (depende do uC) sendo 0 o maior valor de interrupção.
