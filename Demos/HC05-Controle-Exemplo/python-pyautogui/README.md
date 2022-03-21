## Automação (programas)

**Você deve escolher esse exemplo se quiser fazer um controle para aplicativos em geral, exemplo: Youtube, Chrome, Video Player.**

Instale os pacotes necessários do Python via PIP. Os pacotes necessários estão no arquivo `requeriments.txt` (dentro da pasta `python/`):

``` bash
pip3 install -r requirements.txt
```

Utilize o arquivo `youtube_controller.py` como base para o seu controle, ele já vem configurado para usar no Youtube com um botão configurado (botão A do controle ->  tecla L do Youtube para adiantar o vídeo em 10 segundos).

**Primeiro:** vamos testar se o computador instalou a biblioteca de automação corretamente (`pyautogui`).

Execute o arquivo `youtube_controller.py` com os parametros `none -c dummy`. 

``` bash
python3 youtube_controller.py none -c dummy
```

Isto fará com que o aplicativo em Python, inicie sem comunicar com o Bluetooth e irá simular o aperto da tecla A do controle a cada 1 seg. 

Abra um vídeo no Youtube e verifique se o vídeo fica pulando 10s a cada segundo. Dica: para debugar quando ocorre o botão, adicione a flag `-d` no final dos parametros para mostrar mensagens de debug.

### Testando com a placa

Necessitamos agora parear o Bluetooth do computador com o HC-05 e criar uma porta serial virtual. Para isto siga você deve ter realizado o roteiro `Conectando ao HC-05`, depois retorne para este roteiro.

Se estiver tudo certo com o passo anterior, tente agora executar o `youtube_controller.py` com os parametros `PORTA_COM -b 115200` para conectar via serial, o controle agora deve funcionar no Youtube, aperte o botão da placa e veja se o vídeo vai para frente

- `PORTA_COM `: porta do windows que o bluetooth conectou

### Pronto!

Agora você pode seguir o desenvolvimento do seu projeto!