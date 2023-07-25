
## Controle para Jogos

**Você deve escolher esse exemplo se quiser desenvolver um controle para emuladores, Jogos com suporte a DirectInput/Joystick.**

Siga os passos a seguir:

- Instale os pacotes necessários do Python via PIP. Os pacotes necessários estão no arquivo `requeriments.txt` (dentro da pasta `python/`):

``` bash
pip3 install -r requirements.txt
```

Instale o [`VJoy`](http://vjoystick.sourceforge.net/site/) para Windows, no qual permite criarmos um controle virtual programável para comunicarmos via Python: https://sourceforge.net/projects/vjoystick/files/Beta%202.x/2.1.9.1-160719/vJoySetup.exe/download

Copie os arquivos `vJoyInterface.dll` e `vJoyInterfaceWrap.dll` da pasta `Program Files/VJoy` ou `Arquivos de Programas/VJoy` e pasta `x86` ou `x64` (depenendo da sua instalação python) para a `pasta do projeto/PC_Python/pyvjoy`. Para verificar qual versão python instalado execute no cmd do windows:

```bash
python3
```

> Deve aparecer algo com x86 ou x64.

Utilize o arquivo `game_controller.py` como base para o seu controle, ele já vem configurado para usar em um emulador com um botão configurado (botão A do controle -> botão 1 do controle virtual).

Primeiro vamos testar se o computador instalou a biblioteca de controle virtual corretamente (`vjoy`). Execute o arquivo `game_controller.py` com os parametros `none -c dummy`. 

``` bash
python3 youtube_controller.py none -c dummy
```

Isto fará com que o aplicativo em Python, inicie **sem comunicar** com o Bluetooth e irá simular o aperto da tecla A do controle a cada 1seg. 

Abra um emulador com um jogo (exemplo: ZSNES), e lembre-se de configurar o controle no emulador, que deve reconhecer o `VJoy 1`, como controle e clique na opção para trocar um dos botões, ao fazer isto com o aplicativo rodando, o emulador deve configurar aquele botão corretamente:

![](doc/zsnesL.jpg)

Dica: para debugar quando ocorre o botão, adicione a flag `-d` no final dos parametros ao rodar o python para mostrar mensagens de debug. 

O VJoy instala um monitor para verificar o funcionamento, isto pode te auxiliar a detectar erros, você pode encontra-lo no menu Iniciar em VJoy.

### Testando com a placa

Necessitamos agora parear o Bluetooth do computador com o HC-05 e criar uma porta serial virtual. Para isto siga você deve ter realizado o roteiro `Conectando ao HC-05`, depois retorne para este roteiro.

Se estiver tudo certo com o passo anterior, tente agora executar o `game_controller.py` com os parametros `PORTA_COM -b 115200` para conectar via serial, o controle agora deve funcionar no Youtube, aperte o botão da placa e veja se o vídeo vai para frente

- `PORTA_COM `: porta do windows que o bluetooth conectou

### Pronto!

Agora você pode seguir o desenvolvimento do seu projeto!