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

![image](https://user-images.githubusercontent.com/63023972/172082241-6c051a33-2152-4460-af65-0f523dc895ab.png)

Alguns pontos desse diagrama valem ser destacados, como por exemplo, os blocos *SHA Hash Engine* e o *Context Registers* estarem ambos enviando e recebendo informações de um para o outro, o motivo disso é que os registradores passam as mensagens que devem ser realizadas a função Hash e depois recebem ela criptografada para que o monitoramento e a comparação sejam feitos e assim a integridade checada, como vemos nos blocos que estão logo abaixo *Monitoring FSM* e *Integrity Scheduler*.

## **Dependências**
* *Power Management Controller* (PMC)

    O relógico periférico não é diretamente utilizado pelo ICM, portanto é necessário habilitar o relógico do ICM no PMC antes de utilizar o ICM.
    
    ```
    // From module: PMC - Power Management Controller
    #include <pmc.h>
    #include <sleep.h>
    ```

* Fontes de Interrupção

    Como foi supracitado, uma requisição de interrupção é enviada se houver falha na integridade, portanto, é preciso conectar com o *Interrupt Controller*.
    
    ```
    // From module: Interrupt management - SAM implementation
    #include <interrupt.h>
    ```

## **Código**

### **Criando a memória de exemplo**

```   
/* Memory region0 area, string "abc" */
volatile uint32_t message_sha_0[16] = {
    0x80636261,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x18000000
};

/* Memory region1 main list area, string "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq" */
volatile uint32_t message_sha_1_main[16] = {
    0x64636261,
    0x65646362,
    0x66656463,
    0x67666564,
    0x68676665,
    0x69686766,
    0x6a696867,
    0x6b6a6968,
    0x6c6b6a69,
    0x6d6c6b6a,
    0x6e6d6c6b,
    0x6f6e6d6c,
    0x706f6e6d,
    0x71706f6e,
    0x00000080,
    0x00000000
};
```

### Como há envio e recebimento de informações:

```
/**
 *  Configure serial console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console. */
	stdio_serial_init(CONF_UART, &uart_serial_options);
}
```

### Handler para interrupção caso ocorra mismatch:

```
static void reg_dig_mismatch_handler(uint8_t reg_num)
{
	if(reg_num == ICM_REGION_NUM_0) {
		printf( " Memory region0 is modified \n\r");
	} else if(reg_num == ICM_REGION_NUM_1) {
		printf( " Memory region1 is modified \n\r");
	}
}
```

### Main

```
int main(void)
{
	/* ICM configuration */
	struct icm_config icm_cfg;

	/* Initialize the SAM system */
	sysclk_init();
	board_init();
	sleepmgr_init();

	/* Initialize the console uart */
	configure_console();

	/* Output example information */
	printf("-- ICM Example --\r\n");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

	/* ICM initialization */
	icm_cfg.is_write_back= false;
	icm_cfg.is_dis_end_mon = false;
	icm_cfg.is_sec_list_branch = false;
	icm_cfg.bbc = 0;
	icm_cfg.is_auto_mode = false;
	icm_cfg.is_dual_buf = false;
	icm_cfg.is_user_hash = false;
	icm_cfg.ualgo = ICM_SHA_1;
	icm_cfg.hash_area_val = 0;
	icm_cfg.des_area_val = 0;
	icm_init(ICM, &icm_cfg);

	/* Set region0 descriptor */
	reg_descriptor[0].start_addr = (uint32_t)message_sha_0;
	reg_descriptor[0].cfg.is_compare_mode = false;
	reg_descriptor[0].cfg.is_wrap = false;
	reg_descriptor[0].cfg.is_end_mon = false;
	reg_descriptor[0].cfg.reg_hash_int_en = false;
	reg_descriptor[0].cfg.dig_mis_int_en = false;
	reg_descriptor[0].cfg.bus_err_int_en = false;
	reg_descriptor[0].cfg.wrap_con_int_en = false;
	reg_descriptor[0].cfg.ebit_con_int_en = false;
	reg_descriptor[0].cfg.status_upt_con_int_en = false;
	reg_descriptor[0].cfg.is_pro_dly = false;
	reg_descriptor[0].cfg.mem_reg_val = 0;
	reg_descriptor[0].cfg.algo = ICM_SHA_1;
	reg_descriptor[0].tran_size = 0;
	reg_descriptor[0].next_addr = 0;

	/* Set region1 descriptor in main list */
	reg_descriptor[1].start_addr = (uint32_t)message_sha_1_main;
	reg_descriptor[1].cfg.is_compare_mode = false;
	reg_descriptor[1].cfg.is_wrap = false;
	reg_descriptor[1].cfg.is_end_mon = true;
	reg_descriptor[1].cfg.reg_hash_int_en = false;
	reg_descriptor[1].cfg.dig_mis_int_en = false;
	reg_descriptor[1].cfg.bus_err_int_en = false;
	reg_descriptor[1].cfg.wrap_con_int_en = false;
	reg_descriptor[1].cfg.ebit_con_int_en = false;
	reg_descriptor[1].cfg.status_upt_con_int_en = false;
	reg_descriptor[1].cfg.is_pro_dly = false;
	reg_descriptor[1].cfg.mem_reg_val = 0;
	reg_descriptor[1].cfg.algo = ICM_SHA_1;
	reg_descriptor[1].tran_size = 0;
	reg_descriptor[1].next_addr = &reg_descriptor_sec;

	/* Set region1 descriptor in secondary list */
	reg_descriptor_sec.start_addr = (uint32_t)message_sha_1_sec;
	reg_descriptor_sec.tran_size = 0;
	reg_descriptor_sec.next_addr = 0;

	/* Set region descriptor start address */
	icm_set_reg_des_addr(ICM, (uint32_t)&reg_descriptor[0]);

	/* Set hash area start address */
	icm_set_hash_area_addr(ICM, (uint32_t)output_sha);

	/* Enable ICM */
	icm_enable(ICM);

	delay_ms(200);

	/* Set region monitoring mode to compare mode */
	reg_descriptor[0].cfg.is_compare_mode = true;
	reg_descriptor[1].cfg.is_compare_mode = true;

	/* Set callback function for digest mismatch interrupt handler */
	icm_set_callback(ICM, reg_dig_mismatch_handler, ICM_REGION_NUM_0,
			ICM_INTERRUPT_RDM, 1);
	icm_set_callback(ICM, reg_dig_mismatch_handler, ICM_REGION_NUM_1,
			ICM_INTERRUPT_RDM, 1);

	/* Modify memory region value */
	message_sha_0[0] = 0x12345678;
	message_sha_1_sec[0] = 0x12345678;

	/* Enable ICM */
	icm_enable(ICM);

	while (1) {
	}
}
```

Vale notar que como foi dito no início do README, um endereço é predefinido para as comprações, como podemos ver nesse trecho de código extraído da função main:

```
/* Set region descriptor start address */
icm_set_reg_des_addr(ICM, (uint32_t)&reg_descriptor[0]);

/* Set hash area start address */
icm_set_hash_area_addr(ICM, (uint32_t)output_sha);
```

## **Curiosidade**
A função Hash utilizada no ICM pertence ao *Secure Hash Algorithm* (SHA), o qual é compatível com o  American FIPS (Federal Information Processing Standards ou Padrões Federais de Processamento de Informações).
