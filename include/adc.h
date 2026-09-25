/*
 * marichi - cooperative kernel for AVR (R) Mega microcontrollers
 * Copyright (C) 2026  notweerdmonk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

/**
 * @file adc.h
 * @brief Header file for ADC module
 * @author notweerdmonk
 */

#ifndef _ADC_H_
#define _ADC_H_

#include <common.h>
#include <port.h>
#include <kernel.h>

/**
 * @brief Perform a single conversion.
 *
 * In single conversion mode use this function to start a conversion and then
 * read the conversion result as a 16-bit value.
 *
 * @param channel The input channel to select.
 * @return ADC conversion result.
 */
/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN_VALUE(SYS_RES_ADC, 0xffff);
 */
uint16_t adc_single_conversion(uint8_t channel);

/**
 * @brief Perform a single conversion asynchronously.
 *
 * In single conversion mode use this function to start a conversion and then
 * read the conversion result in the ADC interrupt.
 *
 * In free running mode use this function to start the first conversion.
 *
 * @param channel The input channel to select.
 */
/*
 function_wrapper_prologue:
   CHECK_RESOURCE_RETURN(SYS_RES_ADC);
 */
void adc_single_conversion_async(uint8_t channel);

/**
 * @brief Initialize ADC module.
 * @param ps Numeric literal denoting prescaling factor.
 * @param ref String literal denoting reference voltage.
 *
 * @see ADC_SET_PS()
 * @see ADC_SET_REF()
 */
#define ADC_INIT(ps, ref) \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_PS(ps); \
    PORT_ADC_SET_REF(ref); \
    PORT_ADC_ENABLE(); \
  }

/**
 * @brief Enable ADC module,
 */
#define ADC_ENABLE() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_ENABLE(); \
  }

/**
 * @brief Disable ADC module.
 */
#define ADC_DISABLE() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_DISABLE(); \
  }

/**
 * @brief Check if ADC is enabled.
 */
#define ADC_IS_ENABLED() \
  PORT_ADC_IS_ENABLED()

/**
 * @brief Set the prescaling factor for the ADC clock.
 * @param ps Numeric literal denoting prescaling factor.
 * <TABLE>
 * <TR>   <TD>Values</TD>   </TR>
 * <TR>   <TD>2</TD>        </TR>
 * <TR>   <TD>4</TD>        </TR>
 * <TR>   <TD>8</TD>        </TR>
 * <TR>   <TD>16</TD>       </TR>
 * <TR>   <TD>32</TD>       </TR>
 * <TR>   <TD>64</TD>       </TR>
 * <TR>   <TD>128</TD>      </TR>
 * </TABLE>
 *
 * The ADC requires clock frequency between 50kHz and 200kHz. Set the prescler
 * according to the value of the system clock (F_CPU).
 */
#define ADC_SET_PS(ps) \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_PS(ps); \
  }


/**
 * @brief Set the reference voltage for the ADC to AREF pin.
 */
#define ADC_SET_REF_AREF() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_REF_AREF(); \
  }

/**
 * @brief Set the reference voltage for the ADC to AVCC.
 */
#define ADC_SET_REF_AVCC() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_REF_AVCC(); \
  }

/**
 * @brief Set the reference voltage for the ADC to internal 1.1V.
 */
#define ADC_SET_REF_INTERNAL() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_REF_INTERNAL(); \
  }

/**
 * @brief Set the reference voltage for the ADC.
 * @param ref Literal specifying reference voltage.
 * <TABLE>
 * <TR>   <TD>Value</TD>      <TD>Description</TD>        </TR>
 * <TR>   <TD>AREF</TD>       <TD>External AREF pin</TD>  </TR>
 * <TR>   <TD>AVCC</TD>       <TD>AVcc</TD>               </TR>
 * <TR>   <TD>INTERNAL</TD>   <TD>Internal 1.1V</TD>      </TR>
 * </TABLE>
 */
#define ADC_SET_REF(ref) \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_REF(ref); \
  }

/**
 * @brief Select channel 0.
 */
#define ADC_SET_CHAN_0() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_0(); \
  }

/**
 * @brief Select channel 1.
 */
#define ADC_SET_CHAN_1() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_1(); \
  }

/**
 * @brief Select channel 2.
 */
#define ADC_SET_CHAN_2() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_2(); \
  }

/**
 * @brief Select channel 3.
 */
#define ADC_SET_CHAN_3() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_3(); \
  }

/**
 * @brief Select channel 4.
 */
#define ADC_SET_CHAN_4() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_4(); \
  }

/**
 * @brief Select channel 5.
 */
#define ADC_SET_CHAN_5() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_5(); \
  }

/**
 * @brief Select channel 6.
 */
#define ADC_SET_CHAN_6() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_6(); \
  }

/**
 * @brief Select channel 7.
 */
#define ADC_SET_CHAN_7() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_7(); \
  }

/**
 * @brief Select channel 8.
 *
 * Use channel 8 for temperature sensor.
 */
#define ADC_SET_CHAN_8() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_8(); \
  }

/**
 * @brief Select internal 1.1V channel.
 */
#define ADC_SET_CHAN_INTERNAL() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_INTERNAL(); \
  }

/**
 * @brief Select ground channel.
 */
#define ADC_SET_CHAN_GND() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN_GND(); \
  }

/**
 * @brief Select ADC channel between 0 to 8 and 14, 15.
 * @param ch Numeric literal denoting ADC channel.
 * <TABLE>
 * <TR>   <TD>Values</TD>   <TD>Description</TD>          </TR>
 * <TR>   <TD>0</TD>        <TD>Channel 0</TD>            </TR>
 * <TR>   <TD>1</TD>        <TD>Channel 1</TD>            </TR>
 * <TR>   <TD>2</TD>        <TD>Channel 2</TD>            </TR>
 * <TR>   <TD>3</TD>        <TD>Channel 3</TD>            </TR>
 * <TR>   <TD>4</TD>        <TD>Channel 4</TD>            </TR>
 * <TR>   <TD>5</TD>        <TD>Channel 5</TD>            </TR>
 * <TR>   <TD>6</TD>        <TD>Channel 6</TD>            </TR>
 * <TR>   <TD>7</TD>        <TD>Channel 7</TD>            </TR>
 * <TR>   <TD>8</TD>        <TD>Temperature sensor</TD>   </TR>
 * <TR>   <TD>14</TD>       <TD>Internal 1.1V</TD>        </TR>
 * <TR>   <TD>15</TD>       <TD>Ground</TD>               </TR>
 * </TABLE>
 */
#define ADC_SET_CHAN(ch) \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_CHAN(ch); \
  }

/**
 * @brief Read the ADC value.
 * @return The 10-bit ADC value as a 16-bit integer.
 *
 * @see ADC_SET_LAR()
 */
#define ADC_READ() \
  ({ \
    uint16_t adc = check_resource_owner(SYS_RES_ADC) ? \
      PORT_ADC_DATA_REGISTER : 0xffff; \
    adc; \
  })

/**
 * @brief Set 10-bit ADC value to be left adjusted.
 */
#define ADC_SET_LAR() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_LAR(); \
  }

/**
 * @brief Set 10-bit ADC value to be right adjusted.
 */
#define ADC_SET_NO_LAR() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_NO_LAR(); \
  }

/**
 * @brief Start ADC conversion.
 *
 * In Single Conversion mode, use to start each conversion.
 * In Free Running mode, use to start the first conversion,
 * This first conversion performs initialization of the ADC.
 */
#define ADC_START_CONVERSION() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_START_CONVERSION(); \
  }

/**
 * @brief Enable auto trigger for ADC.
 */
#define ADC_ENABLE_AUTO_TRIGGER() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_ENABLE_AUTO_TRIGGER(); \
  }

/**
 * @brief Disable auto trigger for ADC.
 */
#define ADC_DISABLE_AUTO_TRIGGER() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_DISABLE_AUTO_TRIGGER(); \
  }

/**
 * @brief Check if auto trigger is enabled for ADC,
 */
#define ADC_IS_AUTO_TRIGGER_ENABLED() \
  PORT_ADC_IS_AUTO_TRIGGER_ENABLED()

/**
 * @brief Set the ADC to function in free-running mode.
 */
#define ADC_SET_FREE_RUNNING() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_FREE_RUNNING(); \
  }

/**
 * @brief Set the auto-trigger source as analog comparator.
 */
#define ADC_SET_AT_ANALOG_COMPARATOR() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_AT_ANALOG_COMPARATOR(); \
  }

/**
 * @brief Set the auto-trigger source as external interrupt request 0
 */
#define ADC_SET_AT_EXTERNAL_IRQ0() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_AT_EXTERNAL_IRQ0(); \
  }

/**
 * @brief Set the auto-trigger source as timer 0 compare match A.
 */
#define ADC_SET_AT_TIMER0_COMPARE_MATCH_A() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_AT_TIMER0_COMPARE_MATCH_A(); \
  }

/**
 * @brief Set the auto-trigger source as timer 0 overflow.
 */
#define ADC_SET_AT_TIMER0_OVERFLOW() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_AT_TIMER0_OVERFLOW(): \
  }

/**
 * @brief Set the auto-trigger source as timer 1 compare match B.
 */
#define ADC_SET_AT_TIMER1_COMPARE_MATCH_B() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_AT_TIMER1_COMPARE_MATCH_B(); \
  }

/**
 * @brief Set the auto-trigger source as timer 1 overflow.
 */
#define ADC_SET_AT_TIMER1_OVERFLOW() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_AT_TIMER1_OVERFLOW(); \
  }

/**
 * @brief Set the auto-trigger source as timer 1 capture event.
 */
#define ADC_SET_AT_TIMER1_CAPTURE_EVENT() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_SET_AT_TIMER1_CAPTURE_EVENT(); \
  }

/**
 * @brief Enable ADC interrupt.
 */
#define ADC_ENABLE_INTERRUPT() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_ENABLE_INTERRUPT(); \
  }

/**
 * @brief Disable ADC interrupt.
 */
#define ADC_DISABLE_INTERRUPT() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_DISABLE_INTERRUPT(); \
  }

/**
 * @brief Check if ADC interrupt is enabled.
 */
#define ADC_IS_INTERRUPT_ENABLED() \
  PORT_ADC_IS_INTERRUPT_ENABLED()

/**
 * @brief Check if ADC interrupt flag is set.
 */
#define ADC_IS_INTERRUPT_FLAG_SET() \
  PORT_ADC_IS_INTERRUPT_FLAG_SET()

/**
 * @brief Clear the ADC interrupt flag.
 */
#define ADC_CLEAR_INTERRUPT_FLAG() \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_ADC_CLEAR_INTERRUPT_FLAG(); \
  }

/**
 * @brief Disable the the digital input buffer on the corresponding ADC pin.
 * @param pin Numeric literal denoting ADC pin.
 *
 * The corresponding PIN Register bit will always read as zero. When an analog
 * signal is applied to the ADC5...0 pin and the digital input from this pin
 * is not needed, this bit should be written logic one to reduce power
 * consumption in the digital input buffer.
 *
 * Note that ADC pins ADC7 and ADC6 do not have digital input buffers, and
 * therefore do not require Digital Input Disable bits.
 */
#define DIGITAL_INPUT_DISABLE_PIN(pin) \
  if (check_resource_owner(SYS_RES_ADC)) { \
    PORT_DIGITAL_INPUT_DISABLE_PIN(pin); \
  }

/**
 * @brief Enum for allowed prescale factors.
 */
typedef enum _port_adc_prescaler adc_prescaler_t;

/**
 * @brief Enum for allowed ADC voltage reference values.
 */
typedef enum _port_adc_reference adc_reference_t;

/**
 * @brief Set the prescaler for the ADC clock.
 * @param ps Enum specifying the prescaling factor.
 */
DEPRECATED_FUNCTION("Use ADC_SET_PS")
/* function_wrapper_prologue: CHECK_RESOURCE_RETURN(SYS_RES_ADC); */
void adc_set_prescaler(adc_prescaler_t ps);

/**
 * @brief Set the reference voltage for the ADC.
 * @param ref Enum specifying the reference voltage selection.
 */
DEPRECATED_FUNCTION("Use ADC_SET_REF")
/* function_wrapper_prologue: CHECK_RESOURCE_RETURN(SYS_RES_ADC); */
void adc_set_reference(adc_reference_t ref);

/**
 * @brief Changes the input channel for the ADC.
 *
 * In single conversion mode this function may not be uses as
 * adc_single_conversion() takes the channel as input parameter.
 *
 * In auto-trigger mode this function should be used either inside the ADC
 * interrupt or before the trigger event occurs.
 *
 * In free running mode
 *
 * @param channel The input channel to select.
 */
DEPRECATED_FUNCTION("Use ADC_SET_CHAN")
/* function_wrapper_prologue: CHECK_RESOURCE_RETURN(SYS_RES_ADC); */
void adc_set_channel(uint8_t channel);

#endif /* _ADC_H_ */
