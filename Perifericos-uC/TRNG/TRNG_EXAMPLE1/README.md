# True Random Number Generator - TRNG

É um periférico que gera numeros aleatórios de tamanho 32-bit.

## Para que serve:

Um TRNG é uma função ou dispositivo baseado em um fenômeno físico imprevisível, chamado de fonte de entropia, projetado para gerar dados não determinísticos (por exemplo, uma sucessão de números) para semear algoritmos de segurança.

Os dispositivos conectados estão se tornando parte da vida cotidiana e esperamos que funcionem corretamente, protegendo as informações comerciais e pessoais. Os TRNGs estão na base da proteção desses dispositivos, pois são usados para criar e proteger segredos e outras informações confidenciais. Eles fazem parte de uma “cadeia de confiança” que precisa ser estabelecida a partir do SoC, passando para as camadas de aplicação e comunicação para a nuvem. Uma corrente de confiança é tão forte quanto seu elo mais fraco.

Geradores de números aleatórios previsíveis (RNGs) abrem portas para muitos ataques possíveis que podem invadir dispositivos e comprometer dados. Para serem eficazes, os números aleatórios devem ser imprevisíveis, estatisticamente independentes (não relacionados a quaisquer números aleatórios gerados anteriormente), uniformemente distribuídos (igual probabilidade de qualquer número ser gerado) e protegidos.

![image](https://user-images.githubusercontent.com/62663074/171724487-b2a4dfa9-dd34-4ed6-ab44-5fcc74792ae4.png)

## Possiveis aplicações:

- Geração de chaves para vários algoritmos (simétricos, assimétricos, MACs) e protocolos (SSL/TLH, SSH, WiFi, LTE, IPsec, etc.);
- Fabricação de chips (dispositivo de semeadura único e chaves de plataforma);
- Valores iniciais (para algoritmos de criptografia e MAC, valores de pacotes TCP, etc.);
- Geração de Nonce e valores iniciais do contador para várias funções criptográficas;
- Desafios usados para trocas de autenticação de protocolo;
- Etc.;

Diferentemente de 

## Conexão e configuração

- Não é necessário

## Explicação


### Definição das variáveis

<img width="492" alt="image" src="https://user-images.githubusercontent.com/62663074/171494708-27023acf-affa-47ec-84de-287801463986.png">

### Definição das Structs

<img width="483" alt="image" src="https://user-images.githubusercontent.com/62663074/171494830-f9bb7b31-efa1-4838-bd34-c893356d5b8e.png">

### Inicialização e Configuração

<img width="483" alt="image" src="https://user-images.githubusercontent.com/62663074/171494937-db952b1a-177b-4e4d-9564-cb6f0793e175.png">

- Parâmetros da função:
<img width="492" alt="image" src="https://user-images.githubusercontent.com/62663074/171495052-62e131bb-5075-4fac-a787-8f53c8cc8d54.png">
