# Encoder (PIO) - IRQ

Configura o encoder para modificar o numero de voltas dependendo de que direção ele foi girado

## Conexão e configuração

- Conectar o pino [GND] do encoder no pino [GND] da placa
- Conectar o pino [+] do encoder no pino [3V3] da placa
- Conectar o pino [CLK] do encoder no pino [PC13] da placa
- Conectar o pino [DT] do encoder no pino [PD30] da placa
- Conectar o pino [SW] do encoder no pino [PD11] da placa
- Conectar a placa [OLED] no conjunto de pinos [EXT1] da placa

# Encoder PIO - IRQ

Quando o encoder é girado, duas ondas quadradas são produzidas, com seu output em CLK e DT. Quando se gira no sentido horário, a primeira onda é produzida pelo CLK, e na metado do tempo em que CLK permanece em pico, a onda do DT é então produzida, as duas ficando em pico simultaneamente por um quarto de período, quando então a onda de CLK desce para seu valor mínimo (0) enquanto DT continua em pico.

\Para melhor ilustrar isso, veja a imagem a seguir:

![alt text](https://github.com/TiagoSeixas2103/SAME70-examples/blob/master/Sensores/Encoder(PIO)/squareGraphs.png)

Caso o encoder seja girado no sentido antihorário, a onda produzida primeiro será a de DT, seguida da onda produzida por CLK.

# Explicação

Como só precisamos comparar as ondas produzidas por CLK e DT uma vez para saber qual foi a primeira onda produzida, nós só precisamos fazer uma função de callback, que checa no momento em que houve o giro se as duas ondas estão em pico ou não. Por conveniência, nós usamos
o output do CLK, que é produzido primeiro quando giramos o encoder para o sentido horário.

Como comparamos as duas ondas no momento em que CLK chega no seu pico, isso significa que quando CLK = DT, a onda DT foi a primeira a ser produzida, logo o encoder foi girado no sentido anti-horário. Caso CLK != DT no momento em que CLK atingir seu pico, então CLK foi a primeira onda a ser produzida, e portanto o encoder foi girado no sentido horário.

## Definindo a função de callBack

Como mencionado antes, só uma função de callback foi definida, e ela varia um contador que contémo valor de voltas que o encoder deu em relação a sua [posição] [inicial], ou seja, se o encoder for girado 10 vezes no sentido horário

```c
void clk_callback(void){	
	if (pio_get(DT_PIO, PIO_INPUT, DT_IDX_MASK) != pio_get(CLK_PIO, PIO_INPUT, CLK_IDX_MASK)) {
		contador++;
	} else {
		contador--;
	}
}
```

Essa função será chamada sempre que ocorrer uma interrupção em um pino, que será configurado para gerar um interrupção.

