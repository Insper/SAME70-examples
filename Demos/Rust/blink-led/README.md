# Rust - Blink LED

Como utilizar a linguagem Rust para criar programas no processador Cortex M7 (utilizado na place SAME70)

## O que é Rust?

![](https://www.rust-lang.org/static/images/rust-social-wide.jpg)

A linguagem Rust é relativamente nova, pois a primeira versão de código aberto foi lançada em 2010 pela Mozilla. Ela é utilizada para o desenvolvimento de diferentes tipos de aplicações, como ferramentas de linha de comando, web services, ferramentas para DevOps, jogos, aplicativos de IoT — Internet of Things — e muito mais.

As características da linguagem permitem que ela tenha performance suficiente para ser utilizada tanto no desenvolvimento de sistemas de nível mais baixo quanto no de aplicações de alto nível. Não é à toa que ela surgiu como uma alternativa para o C++, entretanto, com algumas características mais modernas, que ajudam a evitar falhas e problemas de segurança.

## Por que Rust?

![](https://miro.medium.com/max/946/1*fxzf9jXmklqPdIBp-xMiZA.png)

### Analise estática poderosa

Garanta a configuração de pinos e periféricos durante o tempo de compilação. Garanta que recursos não serão usados de maneira não intencional por partes da sua aplicação.

### Memória flexível

Alocação dinâmica de memória é opcional. Use um alocador global e estruturas de dados dinâmicas. Ou evite o heap e aloque tudo estaticamente.

### Concorrência sem medo

Rust torna impossível compartilhar estado acidentalmente entre threads. Use o método de concorrência que preferir, e você manterá as fortes garantias de Rust.

### Interoperabilidade

Integre Rust em sua base de código atual em C, ou aproveite SDKs existentes para escrever suas aplicações em Rust

### Portabilidade

Escreva bibliotecas ou drivers uma vez, e use-os em diversos sistemas, desde pequenos microcontroladores até poderosos SBCs.

### Conduzido pela Comunidade

Como parte do projeto Rust de código aberto, o suporte para sistemas embarcados é movido por uma das melhores comunidades abertas, com suporte de parceiros comerciais.

## Ferramental

Lidar com microcontroladores envolve o uso de várias ferramentas diferentes, pois estaremos lidando com uma arquitetura diferente da do seu laptop e teremos que executar e depurar programas em um dispositivo remoto.

Usaremos todas as ferramentas listadas abaixo. Qualquer versão recente deve funcionar quando uma versão mínima não for especificada, mas listamos as versões que testamos.

- Rust
- cargo-binutils
- qemu-system-arm (caso você queira testar sem subir o código para a placa)
- OpenOCD
- GDB com suporte ARM

O texto abaixo explica por que estamos usando essas ferramentas. As instruções de instalação podem ser encontradas na próxima seção.

### cargo-binutils

load-binutils é uma coleção de subcomandos Cargo (gerenciador de pacotes do Rust, tipo o NPM em JS) que facilitam o uso das ferramentas LLVM que são enviadas com a cadeia de ferramentas Rust. Essas ferramentas incluem as versões LLVM de objdump, nm e size e são usadas para inspecionar binários.

### qemu-system-arm

QEMU é um emulador. Nesse caso, usamos a variante que pode emular totalmente os sistemas ARM. Usamos o QEMU para executar programas embutidos no host. Graças a isso, você pode acompanhar algumas partes do tutorial, mesmo que não tenha nenhum hardware com você!

### GDB

Um depurador é um componente muito importante do desenvolvimento incorporado, pois você nem sempre pode se dar ao luxo de registrar coisas no console do host. Em alguns casos, você pode nem ter LEDs para piscar no seu hardware!

### OpenOCD

O GDB não é capaz de se comunicar diretamente com o hardware de depuração em sua placa de desenvolvimento SAME70. Ele precisa de um tradutor e o Open On-Chip Debugger, OpenOCD, é esse tradutor. OpenOCD é um programa que roda em seu laptop.

## Instalação

### Instalação do Rust

- Instale o rustup seguindo as instruções em https://www.rust-lang.org/pt-BR/tools/install
- Após instalar, verifique se a versão do compilador é maior do que a 1.31, com o commando:

```
$ rustc -V
rustc 1.61.0 (fe5b13d68 2022-05-18)
```

- Por questões de largura de banda e uso de disco, a instalação padrão suporta apenas compilação nativa (para o seu x86-64, por exemplo). Para adicionar suporte de compilação cruzada para as arquiteturas ARM Cortex-M, devemos instalar o compilador para nossa arquitetura. Para a placa SAME-70, use o compilador `thumbv7em-none-eabihf`. Para instalar, execute o comando abaixo:

```
Cortex-M4F and M7F with hardware floating point (ARMv7E-M architecture):
$ rustup target add thumbv7em-none-eabihf
```

### Cargo-binutils

```
$ cargo install cargo-binutils

$ rustup component add llvm-tools-preview
```

### GDB e openocd

```
$ sudo apt install gdb-multiarch openocd
```

### Clone o Repositório

- Por fim, clone este repositório com o exemplo do Blink LED

## Conhecendo o Hardware

Antes de começarmos, você precisa identificar algumas características do dispositivo de destino, pois elas serão usadas para configurar o projeto:

- O Core ARM. por exemplo, Cortex-M7.

- O núcleo ARM inclui uma FPU (Floating Point Unit)? Os cores Cortex-M7**F** sim.

- Quanta memória Flash e RAM o dispositivo de destino possui? por exemplo, 256 KiB de Flash e 32 KiB de RAM.

- Onde a memória Flash e a RAM são mapeadas no espaço de endereço? por exemplo, a RAM geralmente está localizada no endereço 0x2000_0000.

Você pode encontrar essas informações na folha de dados ou no manual de referência do seu dispositivo.

Nesta seção, usaremos nosso hardware de referência, o SAME70. Este microcontrolador possui:

- Um núcleo Cortex-M7F que inclui um único FPU de precisão

![](imgs/core.png)

- 2 Mbytes de Flash localizado no endereço 0x00400000.

![](imgs/flash-memory.png)

- 384 KBytes de RAM localizados no endereço 0x20400000. (Existem outras regiões de RAM, mas para simplificar, vamos ignorá-las).

![](imgs/ram-memory.png)

## Configurando

1. O passo número um é definir um destino de compilação padrão em `.cargo/config.toml`. Para fazer isso, descomente o target para a nossa placa (o `thumbv7em-none-eabihf` que tinhamos instalado anteriormente)

```
# Pick ONE of these compilation targets
# target = "thumbv6m-none-eabi"    # Cortex-M0 and Cortex-M0+
# target = "thumbv7m-none-eabi"    # Cortex-M3
# target = "thumbv7em-none-eabi"   # Cortex-M4 and Cortex-M7 (no FPU)
target = "thumbv7em-none-eabihf" # Cortex-M4F and Cortex-M7F (with FPU)
```

Usaremos o `thumbv7em-none-eabihf`, pois cobre o Cortex-M7F.

2. A segunda etapa é inserir as informações da região de memória que vimos anteriormente no manual do SAME-70 no arquivo `memory.x`.

```
/* Memory Spaces Definitions */
MEMORY
{
  FLASH  : ORIGIN = 0x00400000, LENGTH = 0x00200000
  RAM : ORIGIN = 0x20400000, LENGTH = 0x00060000
}
```

## Overview do Programa

Por conveniência, aqui está o código-fonte localizado no diretório `src/main.rs`

```rust
#![no_std]
#![no_main]

use panic_halt as _;
use cortex_m_rt::entry;
use cortex_m::asm;

struct Pio {
    pio_per: *mut u32, // PIO Enable Register
    pio_oer: *mut u32, // PIO Output Enable Register
    pio_sodr: *mut u32, // Set Output Data Register
    pio_codr: *mut u32, // Clear Output Data Register
    pio_puer: *mut u32, // Pull-up Enable Register
    pio_pudr: *mut u32, // Pull-up Disable Register
    pio_ier: *mut u32, // Interrupt Enable Register
    pio_ifscer: *mut u32, // Input Filter Slow Clock Enable Register
    pio_ifscdr: *mut u32, // Input Filter Slow Clock Disable Register
    pio_mder: *mut u32, // Multi-driver Enable Register
    pio_mddr: *mut u32, // Multi-driver Disable Register
}

fn pio_set(p_pio: &Pio, ul_mask: u32) {
    unsafe { *(p_pio.pio_sodr)= ul_mask };
}

fn pio_clear(p_pio: &Pio, ul_mask: u32) {
    unsafe { *(p_pio.pio_codr)= ul_mask };
}

fn pio_pull_up(p_pio: &Pio, ul_mask: u32, ul_pull_up_enable: bool) {
    if ul_pull_up_enable {
        unsafe { *(p_pio.pio_puer) = ul_mask };
    } else {
        unsafe { *(p_pio.pio_pudr) = ul_mask };
    }
}

fn pio_set_input(p_pio: &Pio, ul_mask: u32, ul_attribute: bool) {
    unsafe { *(p_pio.pio_ier) = ul_mask };

    if ul_attribute {
        pio_pull_up(p_pio, ul_mask, true);
        unsafe { *(p_pio.pio_ifscer) = ul_mask };
    } else {
        pio_pull_up(p_pio, ul_mask, false);
        unsafe { *(p_pio.pio_ifscdr) = ul_mask };
    }
}

fn pio_set_output(
    p_pio: &Pio,
    ul_mask: u32,
    ul_default_level: bool,
    ul_multidrive_enable: bool,
    ul_pull_up_enable: bool)
{

    unsafe { *(p_pio.pio_per) = ul_mask };
    unsafe { *(p_pio.pio_oer) = ul_mask };

    if ul_default_level {
        pio_set(p_pio, ul_mask);
    } else {
        pio_clear(p_pio, ul_mask);
    }

    if ul_multidrive_enable {
        unsafe { *(p_pio.pio_mder) = ul_mask };
    } else {
        unsafe { *(p_pio.pio_mddr) = ul_mask };
    }

    pio_pull_up(p_pio, ul_mask, ul_pull_up_enable);
}

fn delay_ms(ms: u32) {
    for _wait in 0..ms {
        asm::nop();
    }
}

#[entry]
fn main() -> ! {

    let pioc = Pio {
        pio_per: 0x400e1200 as *mut u32,
        pio_oer: 0x400e1210 as *mut u32,
        pio_sodr: 0x400E1230 as *mut u32,
        pio_codr: 0x400E1234 as *mut u32,
        pio_puer: 0x400E1264 as *mut u32,
        pio_pudr: 0x400E1260 as *mut u32,
        pio_ier: 0x400E1240 as *mut u32,
        pio_ifscer: 0x400E1284 as *mut u32,
        pio_ifscdr: 0x400E1280 as *mut u32,
        pio_mder: 0x400E1250 as *mut u32,
        pio_mddr: 0x400E1254 as *mut u32,
    };

    let ul_pioc8_mask: u32 = 1 << 8;

    let pioa = Pio {
        pio_per: 0x400E0E00 as *mut u32,
        pio_oer: 0x400E0E10 as *mut u32,
        pio_sodr: 0x400E0E30 as *mut u32,
        pio_codr: 0x400E0E34 as *mut u32,
        pio_puer: 0x400E0E64 as *mut u32,
        pio_pudr: 0x400E0E60 as *mut u32,
        pio_ier: 0x400E0E40 as *mut u32,
        pio_ifscer: 0x400E0E84 as *mut u32,
        pio_ifscdr: 0x400E0E80 as *mut u32,
        pio_mder: 0x400E0E50 as *mut u32,
        pio_mddr: 0x400E0E54 as *mut u32,
    };

    let ul_pioa11_mask: u32 = 1 << 11;

    let wdt_mr: *mut u32  = 0x400e1854 as *mut u32;

    pio_set_output(&pioc, ul_pioc8_mask, false, false, false);
    pio_set_input(&pioa, ul_pioa11_mask, false);


    unsafe {
        *wdt_mr = 0x000080000; // Disable watchdog timer.
    }

    loop {
        pio_clear(&pioc, ul_pioc8_mask);

        delay_ms(100000);

        pio_set(&pioc, ul_pioc8_mask);

        delay_ms(100000);
    }
}

```

Este programa é um pouco diferente de um programa Rust padrão, então vamos dar uma olhada mais de perto.

- `#![no_std]` indica que este programa não será vinculado ao compilador 'std'. Em vez disso, ele será vinculado ao seu subconjunto: o compilador 'core'.

- `#![no_main]` indica que este programa não usará a interface principal padrão que a maioria dos programas Rust usa.

- `use panic_halt as _;` Este padrão fornece um panic_handler que define o comportamento de pânico do programa.

- `#[entry]` é um atributo fornecido pela biblioteca cortex-m-rt que é usada para marcar o ponto de entrada do programa. Como não estamos usando a interface principal padrão, precisamos de outra maneira de indicar o ponto de entrada do programa, que seria #[entry].

- `fn main() -> !` Nosso programa será o único processo em execução no hardware de destino, então não queremos que ele termine! Usamos uma função divergente (o -> ! bit na assinatura da função) para garantir que em tempo de compilação seja esse o caso.

## Compilando o código

Para compilar o código é muito simples, execute o comando:

- Para testar:
  ```
  $ cargo build
  ```
- Para produção:
  ```
  $ cargo build --release
  ```
- Para limpar:
  ```
  $ cargo clean
  ```

Após executar o comando de build, uma pasta target será gerada. Nela, dentro de thumbv7em-none-eabihf, teremos duas opções, debug para o código teste e release para o código de produção.

Dentro de cada uma dessas pastas teremos os arquivos executáveis `blink-led` (sem extensão), que será usado para rodar o código na placa.

## Rodando o código sem Debugger

Primeiro, vá para o caminho onde está o executável:

1. `$ cd target/thumbv7em-none-eabihf/release` ou `$ cd target/thumbv7em-none-eabihf/debug`

2. Execute em um terminal o comando:

`$ openocd -f board/atmel_same70_xplained.cfg -c "init" -c "program blink-led verify reset exit"`

3. Pronto! Seu código agora está rodando na placa!

## Rodando o código com GDB

Para rodar o código com o GDB, devemos primeiro executar o openocd em outro terminal

1. `$ openocd -f board/atmel_same70_xplained.cfg`

Depois executamos o gdb com o script `openocd.gdb` que está nesse repositório

2. `$ gdb -x openocd.gdb target/thumbv7em-none-eabihf/debug/blink-led` ou simplesmente rode `$ cargo run`

## Referências

- [Rust Embedded Book](https://docs.rust-embedded.org/book/intro/index.html)
- [Programming of ARM Cortex-M microcontrollers](https://svenssonjoel.github.io/pages-2021/cortex-m-assembler-0/index.html)
- [Low Level Learning YT channel](https://www.youtube.com/c/LowLevelLearning)
