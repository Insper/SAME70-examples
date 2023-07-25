# True Random Number Generator - TRNG

É um periférico que gera numeros aleatórios de tamanho 32-bit.

## Para que serve:

Um TRNG é uma função ou dispositivo baseado em um fenômeno físico imprevisível, chamado de fonte de entropia, projetado para gerar dados não determinísticos (por exemplo, uma sucessão de números) para semear algoritmos de segurança.

Os dispositivos conectados estão se tornando parte da vida cotidiana e esperamos que funcionem corretamente, protegendo as informações comerciais e pessoais. Os TRNGs estão na base da proteção desses dispositivos, pois são usados para criar e proteger segredos e outras informações confidenciais. Eles fazem parte de uma “cadeia de confiança” que precisa ser estabelecida a partir do SoC, passando para as camadas de aplicação e comunicação para a nuvem. Uma corrente de confiança é tão forte quanto seu elo mais fraco.

Geradores de números aleatórios previsíveis (RNGs) abrem portas para muitos ataques possíveis que podem invadir dispositivos e comprometer dados. Para serem eficazes, os números aleatórios devem ser imprevisíveis, estatisticamente independentes (não relacionados a quaisquer números aleatórios gerados anteriormente), uniformemente distribuídos (igual probabilidade de qualquer número ser gerado) e protegidos.

## Possiveis aplicações:

- Geração de chaves para vários algoritmos (simétricos, assimétricos, MACs) e protocolos (SSL/TLH, SSH, WiFi, LTE, IPsec, etc.);
- Fabricação de chips (dispositivo de semeadura único e chaves de plataforma);
- Valores iniciais (para algoritmos de criptografia e MAC, valores de pacotes TCP, etc.);
- Geração de Nonce e valores iniciais do contador para várias funções criptográficas;
- Desafios usados para trocas de autenticação de protocolo;
- Etc.;

## Comparação:

Existe também dispositivos que criam os números pseudoaleátorios, através de ou aloritmo ou de um dispositivo em si que necessita que uma "semente" seja usada para a geração de uma sequência numérica. Eles não são verdadeiramente aleatórios justamente por necessitar de uma "semente geradora" que gera uma sequência de números que se repetem com um período tão exorbitante, que podemos ignorá-lo. Embora os números gerados pareçam imprevisíveis em uma execução de um programa, a sequência de números é exatamente a mesma de uma execução para a seguinte. Isso ocorre porque a semente inicial é sempre a mesma. Isso é conveniente quando você está depurando um programa, mas é inútil se você quiser que o programa se comporte de forma imprevisível. Se você quiser uma série pseudo-aleatória diferente toda vez que seu programa for executado, você deverá especificar uma semente diferente a cada vez. Para propósitos comuns, basear a semente no tempo atual funciona bem.

Para criptografia ou usos específicos que requerem obrigatoriamente números verdadeiramente aleatórios será nesse caso que será utilizado o nosso periférico pois sempre será garantido um número aleatório que nao depende de nenhuma "semente geradora".

![image](https://user-images.githubusercontent.com/62663074/171724487-b2a4dfa9-dd34-4ed6-ab44-5fcc74792ae4.png)

## Explicação

Abaixo segue uma foto com o diagrama de bloco do funcionamento do periférico TRNG. A interface TRNG pode ser cronometrada através do Power Management Controller (PMC), assim o programador deve primeiro configurar o PMC para habilitar o relógio da interface do usuário TRNG. O relógio da interface do usuário é independente de qualquer relógio que possa ser usado no circuito lógico da fonte de entropia.

<img width="272" alt="image" src="https://user-images.githubusercontent.com/62663074/172056488-2f59127b-b6b5-4454-9c20-0db2aaa4a28f.png">

Para realizar a interrupçãpo a interface TRNG possui uma linha de interrupção conectada ao Interrupt Controller. Para lidar com interrupções, o
O controlador de interrupção deve ser programado antes de configurar o TRNG cujo Id é o 57. Para que esses números sejam gerados, é antes necessário desbloquear o registrador do TRNG com uma `Key` como espécie de chave mestre, essa chave é um HEX -> `0x524E47` que significa RNG em caracteres ascii. Quando utilizada pela primeira vez o registrador é desbloqueado e então permite-se encontrar os números aleatórios. Quando utilizada essa chave pela segunda vez, esse registrador "desliga" e então não é mais possível criar esses númeoros.

## Main(void)

A função `main` desse programa é responsável por inicializar todos os periféricos envolvidos no projeto e também escreve no terminal a saída de um valor aleatório gerado.

``` c
int main(void)
{
	/* Initialize the SAM system */
	sysclk_init();
	board_init();

	/* Configure console UART */
	configure_console();

	/* Output example information */
	printf("-- TRNG Example --\n\r");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

	/* Configure PMC */
	pmc_enable_periph_clk(ID_TRNG);

	/* Enable TRNG */
	trng_enable(TRNG);

	/* Enable TRNG interrupt */
	NVIC_DisableIRQ(TRNG_IRQn);
	NVIC_ClearPendingIRQ(TRNG_IRQn);
	NVIC_SetPriority(TRNG_IRQn, 0);
	NVIC_EnableIRQ(TRNG_IRQn);
	trng_enable_interrupt(TRNG);

	/* User input loop */
	while (1) {
	}
}
```

### Interrupção

Quando houver uma interupção, será chamado o Handler do TRNG e então aperece no terminal o número aleatório gerado.

```C
void TRNG_Handler(void)
{
	uint32_t status;

	status = trng_get_interrupt_status(TRNG);

	if ((status & TRNG_ISR_DATRDY) == TRNG_ISR_DATRDY) {
		printf("-- Random Value: %lx --\n\r", trng_read_output_data(TRNG));
	}
}
```

### Saída no Terminal

Ao rodarmos o código na placa, utilizamos do Terminal Window para visualizar as saídas, pois o código na forma que se encontra, fica gerando números aleatórios sem nenhuma interrupção pois fica constantemente chando o `TRNG_Handler`.

<img width="337" alt="image" src="https://user-images.githubusercontent.com/62663074/172645925-d6e8b7b1-3b06-4f06-9cf4-0a75b6e6f4b2.png">

Para escolher apenas um numero da lista de numeros gerados (quase que instantaneamente) é preciso utilizar de um botão com interrupção e quando for pressionado, ele então chama as funções do TRNG e guarda essa chamada em uma variável e printa.

## True Random Number Generator - TRNG

Ele é um periférico de geração de números aleatórios muito utilizados principalmente o campo da criptografia assim como no próprio campo da computação para geração dos contratos e algoritmos respectivamentes.

Assim que o TRNG é habilitado no registrador de controle (TRNG_CR), o gerador fornece um valor de 32 bits a cada 84 ciclos de clock. A linha de interrupção TRNG pode ser habilitada no TRNG_IER (respectivamente desabilitada no TRNG_IDR). Esta interrupção é definida quando um novo valor aleatório está disponível e é limpo quando o registrador de status (TRNG_ISR) é lido. O sinalizador DATRDY do (TRNG_ISR) é definido quando os dados aleatórios estão prontos para serem lidos no
o registrador de dados de saída de 32 bits (TRNG_ODATA). O modo normal de operação verifica se o sinalizador do registrador de status é igual a 1 antes de ler o registrador de dados de saída quando um valor aleatório de 32 bits é exigido pelo aplicativo de software.

Segue um exemplo visual de como o TRNG funciona:

<img width="405" alt="image" src="https://user-images.githubusercontent.com/62663074/172023626-c8ba8207-ef9b-447b-8353-606c574c170c.png">

Como o TRNG trabalha com o sequências de clock da placa, a geração dos números é extremamente rápida e eficiente, a não ser que a placa esteja com alguma limitação de clock ou muitas tarefas que a sobrecarreguem e deixe o clock lento, gerando menos números por consequência. Agora a Lib em C para geração de pseudo-números aleatórios precisa sempre de uma seed geradora que sempre gera a mesma sequência, isso para casos menores ou apenas de uma pequena depuração de código é rapida e eficaz, porém para casos maiores ou mais complexos como criptografia, ou computação, essa `seed` precisa estar sempre se alterando e para que isso seja feito demora mais tempo que a geração de núemros através do clock da placa com o TRNG. 

















































