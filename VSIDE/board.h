#ifndef __BOARD_H__
#define __BOARD_H__

#include <gpio.h>


// some hardware-specific defines for VS1053 prototyping boards.
//
// define ONE of the following before including this file
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// USE_PROTOTYPING_BOARD -- VS10XX prototyping board
// USE_SIMPLE_DSP_BOARD -- "simple DSP" with host board


//
// --------- DEFINITIONS FOR VS1053 SIMPLE DSP BOARD ------------
//
#ifdef USE_SIMPLE_DSP_BOARD

#define BUTTON_1 (1 << 3) /* Button 1 is at GPIO3 */
#define BUTTON_1_PRESSED GPIO_IS_HIGH(BUTTON_1)
#define CONFIGURE_BUTTON_1 {GPIO_CONFIGURE_AS_INPUT(BUTTON_1);}

#define LED_1 (1 << 4) /* Led 1 is at GPIO4 */
#define LED_2 (1 << 5) /* Led 2 is at GPIO5 */
#define LED_3 (1 << 6) /* Led 3 is at GPIO6 */
#define LED_4 (1 << 7) /* Led 4 is at GPIO7 */

#define SET_LED_1_ON GPIO_SET_HIGH(LED_1)
#define SET_LED_1_OFF GPIO_SET_LOW(LED_1)
#define SET_LED_2_ON GPIO_SET_HIGH(LED_2)
#define SET_LED_2_OFF GPIO_SET_LOW(LED_2)
#define SET_LED_3_ON GPIO_SET_HIGH(LED_3)
#define SET_LED_3_OFF GPIO_SET_LOW(LED_3)
#define SET_LED_4_ON GPIO_SET_HIGH(LED_4)
#define SET_LED_4_OFF GPIO_SET_LOW(LED_4)

#define CONFIGURE_LED_1 {GPIO_CONFIGURE_AS_OUTPUT(LED_1);}
#define CONFIGURE_LED_2 {GPIO_CONFIGURE_AS_OUTPUT(LED_2);}
#define CONFIGURE_LED_3 {GPIO_CONFIGURE_AS_OUTPUT(LED_3);}
#define CONFIGURE_LED_4 {GPIO_CONFIGURE_AS_OUTPUT(LED_4);}
#define CONFIGURE_LEDS {GPIO_CONFUGURE_AS_OUTPUT(LED_1 | LED_2 | LED_3 | LED_4);}

#endif // USE_SIMPLE_DSP


//
// --------- DEFINITIONS FOR VS1053 PROTOTYPING BOARD ------------
//
#ifdef USE_PROTOTYPING_BOARD

// For historical reasons (same PCB is also used for VS1011 and VS1003), the button
// connections in the prototyping board need to be read in a special way:
#define XDCS_PIN (1 << 11) /* button 1 is at xDCS pin */
#define SI_PIN (1 << 10) /* button 2 is at SI pin and via diode D4 also in xDCS pin */
// (button 3 is more difficult to read in the prototyping board, see schematic.)

#define BUTTON_1_PRESSED ((GPIO_IS_LOW(XDCS_PIN)) && (GPIO_IS_HIGH(SI_PIN)))
#define BUTTON_2_PRESSED (GPIO_IS_LOW(SI_PIN))
#define CONFIGURE_BUTTON_1 {} // No need for configuration, xDCS pin is always input
#define CONFIGURE_BUTTON_2 {} // No need for configuration, xDCS and SI pins are always input

// led 1 is at DREQ pin and DREQ pin is special
#define SET_LED_1_ON DREQ_SET_HIGH
#define SET_LED_1_OFF DREQ_SET_LOW

#define CONFIGURE_LED_1 {} // No need for configuration, DREQ pin is always output
#define CONFIGURE_LEDS {} // No need for configuration, DREQ pin is always output

#endif // USE_PROTOTYPING_BOARD



#endif // __BOARD_H__
