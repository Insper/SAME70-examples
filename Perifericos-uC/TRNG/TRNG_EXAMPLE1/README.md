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

Para criptografia ou usos específicos que requerem obrigatoriamente números verdadeiramente aleatórios é que será utilizado o TRNG pois sempre será garantido um número aleatório sempre que nao depende de nenhuma "semente geradora".

![image](https://user-images.githubusercontent.com/62663074/171724487-b2a4dfa9-dd34-4ed6-ab44-5fcc74792ae4.png)

## Main()

A função `main` desse programa é responsável por inicializar todos os periféricos envolvidos no projeto assim como escrever no terminal um exemplo de como é uma saída verdadeira de quando o usuário chamar a função de gerar o TRNG

<img width="303" alt="image" src="https://user-images.githubusercontent.com/62663074/172022706-5cfc33a3-1003-42ed-8f6f-2e521b4723be.png">

