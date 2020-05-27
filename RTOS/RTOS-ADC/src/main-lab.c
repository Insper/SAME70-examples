#include <asf.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "conf_board.h"
#include "conf_uart_serial.h"
#include "maxTouch/maxTouch.h"

#include "fonts/tfont.h"
#include "fonts/sourcecodepro_28.h"
#include "fonts/calibri_36.h"
#include "fonts/arial_72.h"

#include "arm_math.h"
#include "ecg.h"

/************************************************************************/
/* RTOS                                                                  */
/************************************************************************/
#define TASK_MXT_STACK_SIZE            (2*1024/sizeof(portSTACK_TYPE))
#define TASK_MXT_STACK_PRIORITY        (tskIDLE_PRIORITY)

#define TASK_LCD_STACK_SIZE            (6*1024/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY        (tskIDLE_PRIORITY)

#define DACC_CHANNEL        0 // (PB13)
//#define DACC_CHANNEL        1 // (PD00)

//! DAC register base for test
#define DACC_BASE           DACC
//! DAC ID for test
#define DACC_ID             ID_DACC



//uint ecg[] = {2791 ,2653 ,2589 ,2609 ,2722 ,2853 ,2888 ,2853 ,2688 ,2568 ,2663 ,2748 ,2789 ,2818 ,2807 ,2902 ,2918 ,2792 ,2683 ,2636 ,2689 ,2774 ,2760 ,2702 ,2700 ,2720 ,2746 ,2794 ,2780 ,2865 ,2905 ,2831 ,2909 ,2941 ,2869 ,2764 ,2750 ,2818 ,2873 ,2899 ,2925 ,2843 ,2697 ,2784 ,2932 ,2952 ,2954 ,2954 ,2959 ,2950 ,2849 ,2746 ,2695 ,2703 ,2722 ,2736 ,2712 ,2712 ,2653 ,2623 ,2737 ,2793 ,2813 ,2785 ,2730 ,2787 ,2803 ,2795 ,2732 ,2589 ,2513 ,2637 ,2846 ,3112 ,3471 ,3759 ,3858 ,3753 ,3501 ,3138 ,2732 ,2435 ,2312 ,2206 ,2134 ,2220 ,2344 ,2465 ,2547 ,2558 ,2556 ,2504 ,2447 ,2587 ,2707 ,2709 ,2695 ,2622 ,2584 ,2669 ,2689 ,2745 ,2744 ,2708 ,2740 ,2797 ,2773 ,2739 ,2678 ,2681 ,2730 ,2687 ,2673 ,2767 ,2816 ,2876 ,2884 ,2826 ,2808 ,2825 ,2809 ,2803 ,2822 ,2873 ,2907 ,2851 ,2891 ,3010 ,3022 ,3004 ,2972 ,2964 ,3050 ,3104 ,3045 ,2961 ,2937 ,2992 ,3065 ,3028 ,3019 ,3016 ,3007 ,3050 ,3078 ,3118 ,3130 ,3052 ,2990 ,2986 ,2923 ,2849 ,2789 ,2711 ,2736 ,2780 ,2758 ,2667 ,2547 ,2474 ,2504 ,2566 ,2581 ,2571 ,2563 ,2686 ,2718 ,2635 ,2600 ,2574 ,2552 ,2565 ,2530 ,2588 ,2698 ,2660 ,2681 ,2710 ,2647 ,2596 ,2583 ,2641 ,2772 ,2804 ,2763 ,2723 ,2597 ,2566 ,2671 ,2686 ,2751 ,2792 ,2772 ,2801 ,2815 ,2799 ,2807 ,2720 ,2692 ,2724 ,2686 ,2679 ,2703 ,2693 ,2720 ,2735 ,2734 ,2795 ,2815 ,2751 ,2749 ,2802 ,2830 ,2808 ,2715 ,2709 ,2724 ,2640 ,2572 ,2629 ,2686 ,2781 ,2795 ,2795 ,2798 ,2783 ,2780 ,2763 ,2734 ,2749 ,2705 ,2685 ,2794 ,2833 ,2820 ,2815 ,2778 ,2700 ,2658 ,2646 ,2656 ,2688 ,2718 ,2841 ,2930 ,2894 ,2792 ,2754 ,2818 ,2847 ,2769 ,2738 ,2832 ,2814 ,2810 ,2811 ,2732 ,2665 ,2636 ,2724 ,2805 ,2758 ,2724 ,2711 ,2719 ,2836 ,2942 ,2900 ,2835 ,2841 ,2821 ,2874 ,2889 ,2888 ,2948 ,2940 ,2897 ,2886 ,2765 ,2619 ,2562 ,2512 ,2587 ,2691 ,2709 ,2717 ,2711 ,2669 ,2650 ,2676 ,2692 ,2700 ,2641 ,2766 ,2913 ,2897 ,2815 ,2654 ,2593 ,2697 ,2812 ,2987 ,3317 ,3636 ,3838 ,3843 ,3578 ,3185 ,2805 ,2549 ,2431 ,2319 ,2236 ,2342 ,2448 ,2471 ,2412 ,2400 ,2526 ,2617 ,2592 ,2558 ,2541 ,2490 ,2582 ,2642 ,2712 ,2754 ,2689 ,2585 ,2532 ,2597 ,2794 ,2924 ,2840 ,2799 ,2780 ,2681 ,2601 ,2530 ,2545 ,2666 ,2760 ,2799 ,2854 ,2814 ,2782 ,2832 ,2858 ,2931 ,2920 ,2831 ,2792 ,2791 ,2871 ,2949 ,2976 ,2927 ,2921 ,2897 ,2878 ,2892 ,2929 ,3009 ,2988 ,2989 ,3068 ,3071 ,3075 ,3064 ,3020 ,3036 ,3044 ,3017 ,2994 ,2937 ,2888 ,2946 ,2923 ,2825 ,2728 ,2637 ,2655 ,2720 ,2648 ,2583 ,2530 ,2487 ,2507 ,2502 ,2470 ,2495 ,2525 ,2573 ,2603 ,2566 ,2504 ,2464 ,2445 ,2542 ,2608 ,2535 ,2516 ,2527 ,2553 ,2644 ,2632 ,2619 ,2637 ,2565 ,2505 ,2495 ,2549 ,2652 ,2718 ,2677 ,2718 ,2758 ,2734 ,2713 ,2674 ,2680 ,2697 ,2657 ,2630 ,2589 ,2595 ,2653 ,2687 ,2727 ,2742 ,2700 ,2705 ,2715 ,2638 ,2642 ,2692 ,2729 ,2790 ,2821 ,2849 ,2757 ,2640 ,2726 ,2797 ,2742 ,2730 ,2725 ,2676 ,2703 ,2725 ,2731 ,2694 ,2595 ,2598 ,2713 ,2743 ,2742 ,2794 ,2787 ,2813 ,2776 ,2708 ,2718 ,2685 ,2696 ,2788 ,2870 ,2848 ,2790 ,2696 ,2675 ,2777 ,2819 ,2740 ,2663 ,2597 ,2719 ,2792 ,2763 ,2769 ,2763 ,2767 ,2820 ,2834 ,2833 ,2776 ,2671 ,2786 ,2888 ,2908 ,2928 ,2875 ,2869 ,2922 ,2837 ,2810 ,2909 ,2890 ,2856 ,2796 ,2764 ,2802 ,2787 ,2731 ,2718 ,2701 ,2691 ,2733 ,2705 ,2669 ,2759 ,2823 ,2813 ,2729 ,2627 ,2594 ,2608 ,2562 ,2503 ,2486 ,2544 ,2614 ,2570 ,2586 ,2713 ,2907 ,3257 ,3679 ,3972 ,4095 ,3936 ,3600 ,3196 ,2826 ,2530 ,2284 ,2090 ,2018 ,2028 ,2062 ,2297 ,2463 ,2547 ,2634 ,2667 ,2680 ,2725 ,2700 ,2709 ,2768 ,2790 ,2702 ,2575 ,2525 ,2578 ,2661 ,2672 ,2671 ,2630 ,2585 ,2722 ,2746 ,2746 ,2752 ,2755 ,2845 ,2905 ,2879 ,2822 ,2726 ,2757 ,2841 ,2774 ,2768 ,2843 ,2883 ,2898 ,2874 ,2803 ,2732 ,2737 ,2903 ,3061 ,3094 ,3073 ,2994 ,2854 ,2909 ,3080 ,3158 ,3171 ,3084 ,3056 ,3158 ,3194 ,3143 ,3077 ,3067 ,3089 ,3113 ,3089 ,3083 ,2940 ,2794 ,2754 ,2732 ,2748 ,2747 ,2668 ,2655 ,2589 ,2520 ,2494 ,2526 ,2508 ,2541 ,2621 ,2598 ,2596 ,2616 ,2610 ,2612 ,2578 ,2581 ,2625 ,2583 ,2663 ,2726 ,2681 ,2596 ,2535 ,2536 ,2593 ,2601 ,2625 ,2702 ,2717 ,2731 ,2733 ,2624 ,2528 ,2555 ,2600 ,2680 ,2722 ,2733 ,2729 ,2716 ,2787 ,2864 ,2797 ,2652 ,2572 ,2595 ,2703 ,2755 ,2676 ,2590 ,2559 ,2609 ,2745 ,2785 ,2784 ,2762 ,2689 ,2729 ,2751 ,2687 ,2676 ,2623 ,2581 ,2650 ,2768 ,2845 ,2840 ,2731 ,2664 ,2683 ,2677 ,2680 ,2672 ,2708 ,2812 ,2925 ,2940 ,2835 ,2738 ,2735 ,2738 ,2694 ,2666 ,2681 ,2696 ,2725 ,2723 ,2736 ,2722 ,2687 ,2682 ,2749 ,2772 ,2781 ,2804 ,2784 ,2746 ,2715 ,2759 ,2858 ,2907 ,2923 ,2972 ,2916 ,2881 ,2872 ,2927 ,2984 ,3010 ,2924 ,2848 ,2735 ,2712 ,2846 ,2881 ,2760 ,2597 ,2560 ,2688 ,2804 ,2809 ,2818 ,2765 ,2671 ,2681 ,2711 ,2694 ,2712 ,2786 ,2826 ,2799 ,2704 ,2614 ,2509 ,2498 ,2645 ,2831 ,2972 ,3191 ,3473 ,3796 ,4039 ,4012 ,3714 ,3340 ,2935 ,2634 ,2464 ,2374 ,2297 ,2242 ,2265 ,2446 ,2574 ,2614 ,2644 ,2605 ,2515 ,2502 ,2542 ,2621 ,2681 ,2743 ,2828 ,2868 ,2813 ,2789 ,2750 ,2716 ,2779 ,2824 ,2840 ,2850 ,2773 ,2832 ,2873 ,2825 ,2876 ,2898 ,2877 ,2918 ,2843 ,2741 ,2724 ,2713 ,2843 ,2969 ,2924 ,2896 ,2895 ,2958 ,3042 ,3027 ,2983 ,3074 ,3112 ,3170 ,3227 ,3164 ,3062 ,3056 ,3105 ,3208 ,3209 ,3172 ,3130 ,3065 ,3110 ,3230 ,3228 ,3175 ,3107 ,3101 ,3151 ,3137 ,3018 ,2967 ,2914 ,2896 ,2873 ,2746 ,2711 ,2761 ,2720 ,2685 ,2669 ,2605 ,2536 ,2495 ,2512 ,2576 ,2530 ,2483 ,2519 ,2552 ,2559 ,2656 ,2648 ,2603 ,2581 ,2497 ,2541 ,2621 ,2668 ,2687 ,2673 ,2708 ,2736 ,2655 ,2609 ,2646 ,2653 ,2654 ,2581 ,2576 ,2642 ,2639 ,2644 ,2677 ,2711 ,2779 ,2887 ,2900 ,2926 ,2909 ,2781 ,2700 ,2670 ,2681 ,2750 ,2742 ,2709 ,2714 ,2719 ,2681 ,2583 ,2515 ,2610 ,2663 ,2668 ,2748 ,2808 ,2854 ,2873 ,2841 ,2889 ,2894 ,2842 ,2799 ,2768 ,2776 ,2845 ,2871 ,2835 ,2800 ,2747 ,2782 ,2804 ,2740 ,2716 ,2725 ,2716 ,2803 ,2857 ,2798 ,2754 ,2710 ,2705 ,2755 ,2792 ,2839 ,2874 ,2865 ,2915 ,2894 ,2788 ,2744 ,2755 ,2863 ,2993 ,3027 ,2999 ,2960 ,2879 ,2856 ,2862 ,2876 ,2911 ,2844 ,2791 ,2783 ,2774 ,2759 ,2715 ,2703 ,2737 ,2812 ,2803 ,2721 ,2689 ,2732 ,2770 ,2736 ,2676 ,2653 ,2708 ,2778 ,2893 ,2874 ,2767 ,2603 ,2548 ,2653 ,2743 ,2781 ,2913 ,3170 ,3522 ,3894 ,4039 ,3928 ,3672 ,3338 ,2960 ,2607 ,2311 ,2191 ,2176 ,2236 ,2373 ,2487 ,2541 ,2583 ,2577 ,2617 ,2710 ,2697 ,2625 ,2608 ,2617 ,2686 ,2684 ,2735 ,2837 ,2814 ,2865 ,2930 ,2865 ,2821 ,2776 ,2726 };

typedef struct {
  uint x;
  uint y;
} touchData;

QueueHandle_t xQueueTouch;

typedef struct {
  uint value;
} adcData;

typedef struct {
  uint raw;
  uint fir;
} t_plot;

QueueHandle_t xQueueADC, xQueuePlot;
SemaphoreHandle_t xSemaphore;

/************************************************************************/
/* LCD + TOUCH                                                          */
/************************************************************************/
#define MAX_ENTRIES        3

struct ili9488_opt_t g_ili9488_display_opt;

#define AFEC_POT AFEC1
#define AFEC_POT_ID ID_AFEC1
#define AFEC_POT_CHANNEL 6 // Canal do pino PC31

/** The conversion data is done flag */
volatile bool g_is_conversion_done = false;

/** The conversion data value */
volatile uint32_t g_ul_value = 0;

volatile uint32_t g_ul_dac = 0;


void TC1_Handler(void){
  volatile uint32_t ul_dummy;

  ul_dummy = tc_get_status(TC0, 1);

  /* Avoid compiler warning */
  UNUSED(ul_dummy);

  /* Selecina canal e inicializa conversão */
  afec_channel_enable(AFEC_POT, AFEC_POT_CHANNEL);
  afec_start_software_conversion(AFEC_POT);

}

static void AFEC_pot_Callback(void){
  adcData adc;
  adc.value = afec_channel_get_value(AFEC_POT, AFEC_POT_CHANNEL);
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(xQueueADC, &adc, &xHigherPriorityTaskWoken);
}

static void config_AFEC_pot(Afec *afec, uint32_t afec_id, uint32_t afec_channel, afec_callback_t callback){
  /*************************************
  * Ativa e configura AFEC
  *************************************/
  /* Ativa AFEC - 0 */
  afec_enable(afec);

  /* struct de configuracao do AFEC */
  struct afec_config afec_cfg;

  /* Carrega parametros padrao */
  afec_get_config_defaults(&afec_cfg);

  /* Configura AFEC */
  afec_init(afec, &afec_cfg);

  /* Configura trigger por software */
  afec_set_trigger(afec, AFEC_TRIG_SW);

  /*** Configuracao específica do canal AFEC ***/
  struct afec_ch_config afec_ch_cfg;
  afec_ch_get_config_defaults(&afec_ch_cfg);
  afec_ch_cfg.gain = AFEC_GAINVALUE_0;
  afec_ch_set_config(afec, afec_channel, &afec_ch_cfg);

  /*
  * Calibracao:
  * Because the internal ADC offset is 0x200, it should cancel it and shift
  down to 0.
  */
  afec_channel_set_analog_offset(afec, afec_channel, 0x200);

  /***  Configura sensor de temperatura ***/
  struct afec_temp_sensor_config afec_temp_sensor_cfg;

  afec_temp_sensor_get_config_defaults(&afec_temp_sensor_cfg);
  afec_temp_sensor_set_config(afec, &afec_temp_sensor_cfg);
  
  /* configura IRQ */
  afec_set_callback(afec, afec_channel,	callback, 1);
  NVIC_SetPriority(afec_id, 4);
  NVIC_EnableIRQ(afec_id);
}

/************************************************************************/
/* RTOS hooks                                                           */
/************************************************************************/

/**
* \brief Called if stack overflow during execution
*/
extern void vApplicationStackOverflowHook(xTaskHandle *pxTask,
signed char *pcTaskName)
{
  printf("stack overflow %x %s\r\n", pxTask, (portCHAR *)pcTaskName);
  /* If the parameters have been corrupted then inspect pxCurrentTCB to
  * identify which task has overflowed its stack.
  */
  for (;;) {
  }
}

/**
* \brief This function is called by FreeRTOS idle task
*/
extern void vApplicationIdleHook(void)
{
}

/**
* \brief This function is called by FreeRTOS each tick
*/
extern void vApplicationTickHook(void)
{
}

extern void vApplicationMallocFailedHook(void)
{
  /* Called if a call to pvPortMalloc() fails because there is insufficient
  free memory available in the FreeRTOS heap.  pvPortMalloc() is called
  internally by FreeRTOS API functions that create tasks, queues, software
  timers, and semaphores.  The size of the FreeRTOS heap is set by the
  configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

  /* Force an assert. */
  configASSERT( ( volatile void * ) NULL );
}

/************************************************************************/
/* init                                                                 */
/************************************************************************/

static void configure_lcd(void){
  /* Initialize display parameter */
  g_ili9488_display_opt.ul_width = ILI9488_LCD_WIDTH;
  g_ili9488_display_opt.ul_height = ILI9488_LCD_HEIGHT;
  g_ili9488_display_opt.foreground_color = COLOR_CONVERT(COLOR_WHITE);
  g_ili9488_display_opt.background_color = COLOR_CONVERT(COLOR_WHITE);

  /* Initialize LCD */
  ili9488_init(&g_ili9488_display_opt);
  ili9488_set_display_direction(PORTRAIT);
}

/************************************************************************/
/* funcoes                                                              */
/************************************************************************/

void draw_screen(void) {
  ili9488_set_foreground_color(COLOR_CONVERT(COLOR_WHITE));
  ili9488_draw_filled_rectangle(0, 0, ILI9488_LCD_WIDTH-1, ILI9488_LCD_HEIGHT-1);
}

uint32_t convert_axis_system_x(uint32_t touch_y) {
  // entrada: 4096 - 0 (sistema de coordenadas atual)
  // saida: 0 - 320
  return ILI9488_LCD_WIDTH - ILI9488_LCD_WIDTH*touch_y/4096;
}

uint32_t convert_axis_system_y(uint32_t touch_x) {
  // entrada: 0 - 4096 (sistema de coordenadas atual)
  // saida: 0 - 320
  return ILI9488_LCD_HEIGHT*touch_x/4096;
}

void font_draw_text(tFont *font, const char *text, int x, int y, int spacing) {
  char *p = text;
  while(*p != NULL) {
    char letter = *p;
    int letter_offset = letter - font->start_char;
    if(letter <= font->end_char) {
      tChar *current_char = font->chars + letter_offset;
      ili9488_draw_pixmap(x, y, current_char->image->width, current_char->image->height, current_char->image->data);
      x += current_char->image->width + spacing;
    }
    p++;
  }
}

void mxt_handler(struct mxt_device *device, uint *x, uint *y)
{
  /* USART tx buffer initialized to 0 */
  uint8_t i = 0; /* Iterator */

  /* Temporary touch event data struct */
  struct mxt_touch_event touch_event;
  
  /* first touch only */
  uint first = 0;

  /* Collect touch events and put the data in a string,
  * maximum 2 events at the time */
  do {

    /* Read next next touch event in the queue, discard if read fails */
    if (mxt_read_touch_event(device, &touch_event) != STATUS_OK) {
      continue;
    }
    
    /************************************************************************/
    /* Envia dados via fila RTOS                                            */
    /************************************************************************/
    if(first == 0 ){
      *x = convert_axis_system_x(touch_event.y);
      *y = convert_axis_system_y(touch_event.x);
      first = 1;
    }
    
    i++;

    /* Check if there is still messages in the queue and
    * if we have reached the maximum numbers of events */
  } while ((mxt_is_message_pending(device)) & (i < MAX_ENTRIES));
}

void TC_init(Tc * TC, int ID_TC, int TC_CHANNEL, int freq){
  uint32_t ul_div;
  uint32_t ul_tcclks;
  uint32_t ul_sysclk = sysclk_get_cpu_hz();

  pmc_enable_periph_clk(ID_TC);

  tc_find_mck_divisor(freq, ul_sysclk, &ul_div, &ul_tcclks, ul_sysclk);
  tc_init(TC, TC_CHANNEL, ul_tcclks | TC_CMR_CPCTRG);
  tc_write_rc(TC, TC_CHANNEL, (ul_sysclk / ul_div) / freq);

  NVIC_SetPriority((IRQn_Type) ID_TC, 4);
  NVIC_EnableIRQ((IRQn_Type) ID_TC);
  tc_enable_interrupt(TC, TC_CHANNEL, TC_IER_CPCS);

  tc_start(TC, TC_CHANNEL);
}

/************************************************************************/
/* tasks                                                                */
/************************************************************************/

void task_mxt(void){
  
  struct mxt_device device; /* Device data container */
  mxt_init(&device);       	/* Initialize the mXT touch device */
  touchData touch;          /* touch queue data type*/
  
  while (true) {
    /* Check for any pending messages and run message handler if any
    * message is found in the queue */
    if (mxt_is_message_pending(&device)) {
      mxt_handler(&device, &touch.x, &touch.y);
      xQueueSend( xQueueTouch, &touch, 0);           /* send mesage to queue */
    }
    vTaskDelay(100);
  }
}

void task_lcd(void){
  xQueueTouch = xQueueCreate( 10, sizeof( touchData ) );
  xQueuePlot = xQueueCreate( 10, sizeof( t_plot ) );

  // inicializa LCD e pinta de branco
  configure_lcd();
  draw_screen();

  // strut local para armazenar msg enviada pela task do mxt
  touchData touch;
  t_plot plot;
    
  char buffer[64];

  while (true) {
    if (xQueueReceive( xQueueTouch, &(touch), ( TickType_t )  0 / portTICK_PERIOD_MS)) {
      //printf("Touch em: x:%d y:%d\n", touch.x, touch.y);
    }
    
    if (xQueueReceive( xQueuePlot, &(plot), ( TickType_t )  100 / portTICK_PERIOD_MS)) {
        sprintf(buffer, "%04d", plot.raw );
        font_draw_text(&calibri_36, buffer, 0, 0, 1);        
    }
    
  }
}

void task_adc(void){

  xQueueADC  = xQueueCreate( 100, sizeof( adcData ) );
  
  if (xSemaphore == NULL)
  printf("falha em criar o semaforo \n");
  
  // configura ADC e TC para controlar a leitura
  config_AFEC_pot(AFEC_POT, AFEC_POT_ID, AFEC_POT_CHANNEL, AFEC_pot_Callback);
  TC_init(TC0, ID_TC1, 1, 60);
  
  adcData adc; 
  uint i = 0;
  t_plot plot;
  
  while(1){
    if(xQueueReceive( xQueueADC, &(adc), 1000)){
      plot.raw = (int) adc.value;
      plot.fir = (int) 0;
      xQueueSend(xQueuePlot, &plot, 0);
    }
    
    //     if (xQueueReceive( xQueueADC, &(adc), 100)) {
    //       if(i <= NUM_TAPS){
    //         inputF32[i++] = (float) adc.value;
    //       } else{
    //          arm_fir_f32(&S, &inputF32[0], &outputF32[0], BLOCK_SIZE);
    //          plot.raw = (int) inputF32[0];
    //          plot.fir = (int) outputF32[0];
    //          xQueueSend(xQueuePlot, &plot, 0);
    //          i = 0;
    //       }
    //   	}
  }
}

/************************************************************************/
/* main                                                                 */
/************************************************************************/

int main(void)
{
  /* Initialize the USART configuration struct */
  const usart_serial_options_t usart_serial_options = {
    .baudrate     = USART_SERIAL_EXAMPLE_BAUDRATE,
    .charlength   = USART_SERIAL_CHAR_LENGTH,
    .paritytype   = USART_SERIAL_PARITY,
    .stopbits     = USART_SERIAL_STOP_BIT
  };

  sysclk_init(); /* Initialize system clocks */
  board_init();  /* Initialize board */
  
  /* Initialize stdio on USART */
  stdio_serial_init(USART_SERIAL_EXAMPLE, &usart_serial_options);
  
  /* Create task to handler touch */
  if (xTaskCreate(task_mxt, "mxt", TASK_MXT_STACK_SIZE, NULL, TASK_MXT_STACK_PRIORITY, NULL) != pdPASS) {
    printf("Failed to create test led task\r\n");
  }
  
  /* Create task to handler LCD */
  if (xTaskCreate(task_lcd, "lcd", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
    printf("Failed to create test led task\r\n");
  }
  
  if (xTaskCreate(task_adc, "adc", TASK_LCD_STACK_SIZE, NULL, 0, NULL) != pdPASS) {
    printf("Failed to create test adc task\r\n");
  }
  
  /* Start the scheduler. */
  vTaskStartScheduler();

  while(1){

  }


  return 0;
}
