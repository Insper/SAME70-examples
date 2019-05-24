/**
 * @file pio_insper.h
 * @author Rafael Corsi
 * @date 22/3/2016
 * @brief Funções para configurar o PIO do SAME70
 */

#ifndef PIO_INSPER_H
#define PIO_INSPER_H

/*****************
 * Includes
 *****************/

/**
 * @brief STDINT possui as definições dos tipos de variáveis
 * e constantes
 */
#include <stdint.h>

/**
 * @brief Inclui as definições prévias do uc em uso
 */
#include <same70_xplained.h>

/***************** 
 * Defines
 *****************/

/**
 * Esses defines serão utilizados para configurar
 * o modo de operação dos I/Os, quando em modo
 * de entrada
 */

/*  Default pin configuration (no attribute). */
#define PIO_DEFAULT             (0u << 0)

/*  The internal pin pull-up is active. */
#define PIO_PULLUP              (1u << 0)

/*  The internal glitch filter is active. */
#define PIO_DEGLITCH            (1u << 1)

/*  The pin is open-drain. */
#define PIO_OPENDRAIN           (1u << 2)

/*  The internal debouncing filter is active. */
#define PIO_DEBOUNCE            (1u << 3)


/***************** 
 * PROTOTYPES 
 *****************/

/**
 * \brief Configure one or more pin(s) of a PIO controller as outputs, with
 * the given default value. 
 * 
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask indicating which pin(s) to configure.
 * \param ul_default_level Default level on the pin(s).
 * \param ul_pull_up_enable Indicates if the pin shall have its pull-up
 * activated.
 */
void _pio_set_output(	Pio *p_pio, 
                    	const uint32_t ul_mask,
		       	const uint32_t ul_default_level,
		       	const uint32_t ul_pull_up_enable);




/**
 * \brief Configure one or more pin(s) or a PIO controller as inputs.
 * Optionally, the corresponding internal pull-up(s) and glitch filter(s) can
 * be enabled.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask indicating which pin(s) to configure as input(s).
 * \param ul_attribute PIO attribute(s).
 */
void _pio_set_input( 	Pio *p_pio, 
                    	const uint32_t ul_mask,
            	   	const uint32_t ul_attribute);

/**
 * \brief Configure PIO internal pull-up.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_pull_up_enable Indicates if the pin(s) internal pull-up shall be
 * configured.
 */
void _pio_pull_up(	Pio *p_pio, 
               const uint32_t ul_mask,
		            const uint32_t ul_pull_up_enable);

/**
 * \brief Configure PIO pin internal pull-down.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 * \param ul_pull_down_enable Indicates if the pin(s) internal pull-down shall
 * be configured.
 */
void _pio_pull_down( Pio *p_pio, 
                    const uint32_t ul_mask,
	            	const uint32_t ul_pull_down_enable);

/**
 * \brief Set a high output level on all the PIOs defined in ul_mask.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will save the value if they are changed to outputs.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void _pio_set(   Pio *p_pio, 
                const uint32_t ul_mask);

/**
 * \brief Set a low output level on all the PIOs defined in ul_mask.
 * This has no immediate effects on PIOs that are not output, but the PIO
 * controller will save the value if they are changed to outputs.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s) to configure.
 */
void _pio_clear( Pio *p_pio, 
                const uint32_t ul_mask); 

 /**
 * \brief Return 1 if one or more PIOs of the given Pin are configured to
 * output a high level (even if they are not output).
 * To get the actual value of the pin, use PIO_Get() instead.
 *
 * \param p_pio Pointer to a PIO instance.
 * \param ul_mask Bitmask of one or more pin(s).
 *
 * \retval 1 At least one PIO is configured to output a high level.
 * \retval 0 All PIOs are configured to output a low level.
 */
uint32_t _pio_get_output_data_status(const Pio *p_pio,
                                     const uint32_t ul_mask);

#endif
