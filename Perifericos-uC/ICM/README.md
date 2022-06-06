# **Integrity Check Monitor (ICM)**

## **Contextualizando**
Antes de entender como o periférico ICM funciona, vamos contextualizar brevemente um conceito que será importante durante esse tutorial, a função Hash.

### **Função Hash**
A função Hash é um método de criptografia que resume grandes strings/palavras em um código que não é reversível. Por exemplo, pelo fato de existirem vários algoritmos Hash, um texto qualquer possui diversos tipos de ser criptografado. E isso permite com que ele seja largamente utilizado quando tratamos no quesito segurança. Antes de continuar, vamos entender alguns exemplos de como ele pode ser utilizado:
* Busca de elementos em base de dados;
* Autenticação de senha;
* Checagem de integridade.

## **Funcionamento**
Agora que sabemos como a função Hash funciona, podemos voltar para o ICM. Esse periférico serve para verificar a integridade de regiões de memórias, para caso essa verificação não seja segura, ele possa gerar uma requisição de interrupção. Assim, seu funcionamento consiste em duas formas de operações, uma na qual ele possa realizar a função hash na lista de memória de uma região e a outra na qual ele possa comparar esse código criptografado com um endereço de memótira predefinido. Nesse contexto, caso essa comparação não mostre alguma diferença, uma requisição de interrupção é enviada, caso contrário, tudo ocorre normalmente.
Por fim, vale ressaltar que como o processo de checagem de integridade, necessita receber e enviar informações sem um CPU, ele é um DMA (Direct Memory Access) controller.

## **Diagrama de Blocos**
Na imagem a seguir podemos ver um diagrama de blocos que resume o funcionamento desse periférico:

Alguns pontos desse diagrama valem ser destacados, como por exemplo, os blocos *SHA Hash Engine* e o *Context Registers* estarem ambos enviando e recebendo informações de um para o outro, o motivo disso é que os registradores passam as mensagens que devem ser realizadas a função Hash e depois recebem ela criptografada para que o monitoramento e a comparação sejam feitos e assim a integridade checada, como vemos nos blocos que estão logo abaixo *Monitoring FSM* e *Integrity Scheduler*.

## **Dependências**
* *Power Management Controller* (PMC)

    O relógico periférico não é diretamente utilizado pelo ICM, portanto é necessário habilitar o relógico do ICM no PMC antes de utilizar o ICM.

* Fontes de Interrupção

    Como foi supracitado, uma requisição de interrupção é enviada se houver falha na integridade, portanto, é preciso conectar com o *Interrupt Controller*.