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


#ifndef _KERNEL_H_
#define _KERNEL_H_

/**
 * \defgroup api Public API
 * @{
 */

/**
 * @file kernel.h
 * @author notweerdmonk
 * @brief header file for kernel module
 */

#include <config.h>
#include <common.h>
#include <utility.h>
#include <port.h>

/*****************************************************************************/

typedef uint8_t task_id_t;

typedef uint8_t timer_id_t;

typedef uint8_t event_id_t;

typedef union _task_data {
  uint8_t id;
  int n;
  unsigned int u;
  void *ptr;
} task_data_t;

typedef void* (*task_handler)(task_data_t);

typedef void (*event_handler)(event_id_t, task_data_t);

typedef void (*timer_handler)(timer_id_t, task_data_t);

typedef enum _event_trigger {
  EV_NOW,
  EV_DEFER
} event_trigger_t;

typedef enum _resource_id {
  SYS_RES_TIMER0,
  SYS_RES_TIMER1,
  SYS_RES_TIMER2,
  SYS_RES_UART,
  SYS_RES_LCD,
  SYS_RES_ADC,
  SYS_RES_SPI,
  SYS_RES_TWI,
  SYS_RES_MAX
} resource_id_t;

typedef enum _pwm_channel_id {
  PWM_CHANNEL_A,
  PWM_CHANNEL_B,
} pwm_channel_id_t;

/* TODO: add more traps, traps from SREG */
/* trap events */
typedef enum _trap_type {
  TRAP_TYPE_NONE,
  TRAP_TYPE_RES_CHK,
  TRAP_TYPE_TASK_TIMEOUT
} trap_t;

typedef enum _error {
  ERR_INVALID_RETURN = 1,
  ERR_INVALID_ARGUMENT,
  ERR_RESOURCE_DENIED,
  ERR_OUT_OF_MEMORY,
  ERR_DEBUG
} error_t;

/*****************************************************************************/
/* EEPROM error conditions addresses */
#define EEPROM_ERROR_FLAG_ADDR \
  (uint8_t*)PORT_EEPROM_ERROR_FLAG_ADDR

#define EEPROM_ERROR_VALUE_ADDR \
  (uint8_t*)PORT_EEPROM_ERROR_VALUE_ADDR

#define EEPROM_PC_VALUE_HIGH_ADDR \
  (uint8_t*)PORT_EEPROM_PC_VALUE_HIGH_ADDR

#define EEPROM_PC_VALUE_LOW_ADDR \
  (uint8_t*)PORT_EEPROM_PC_VALUE_LOW_ADDR
/*****************************************************************************/

/*****************************************************************************/
#define SLEEP_MODE_IS_IDLE() \
  PORT_SLEEP_MODE_IS_IDLE()

#define SLEEP_MODE_IS_ADC_NOISE_RED() \
  PORT_SLEEP_MODE_IS_ADC_NOISE_RED()

#define SLEEP_MODE_IS_POWER_DOWN() \
  PORT_SLEEP_MODE_IS_POWER_DOWN()

#define SLEEP_MODE_IS_POWER_SAVE() \
  PORT_SLEEP_MODE_IS_POWER_SAVE()

#define SLEEP_MODE_IS_STANDBY() \
  PORT_SLEEP_MODE_IS_STANDBY()

#define SLEEP_MODE_IS_EXTERNAL_STANDBY() \
  PORT_SLEEP_MODE_IS_EXTERNAL_STANDBY()
/*****************************************************************************/

/**
 * @brief Stop clock to individual peripherals to reduce power consumption.
 * @param res Resource for which clock will be stopped.
 *
 * Resources used by the peripheral when stopping the clock will remain
 * occupied, hence the peripheral should in most cases be disabled before
 * stopping the clock.
 *
 * Module shutdown can be used in Idle mode and Active mode to significantly
 * reduce the overall power consumption. In all other sleep modes, the clock
 * is already stopped.
 */
void set_power_reduction(resource_id_t res);

/**
 * @brief
 */
void tasks_main(void) __attribute__((weak));

/*****************************************************************************/


/**
 * @brief Register a task with the kernel.
 * @param handler The function to be executed by the task.
 * @param app_data User data to be passed to the task function.
 * @param is_service Boolean flag to denote if the task is a service.
 *
 * When a task is designated to be a service, it cannot be suspended/resumed or
 * restarted by the debugger.
 */
task_id_t register_task(task_handler handler, task_data_t app_data,
    boolean is_service);

/**
 * @brief Deregister a task and free up its resources.
 * @param id Identifier of the task to be deregistered.
 */
void deregister_task(task_id_t id);

/**
 * @brief The task should use alive to notify the kernel that it is not frozen.
 *
 * There can be scenarios where the task takes longer than the timeout to
 * perform some action. Inorder to avoid timeout trap the task can use this
 * function to tell the kernel that it is alive.
 */
void alive();

/**
 * @brief Put the task to sleep for given microseconds.
 *
 * The functions sleep and wait_on_event will change the state of task to
 * TASK_STATE_SLEEPING and TASK_STATE_WAITING respectively. Consider yielding
 * or returning from coroutine after calling sleep or wait_on_event. Yielding
 * will resume execution from the same point when the task returns to
 * TASK_STATE_RUNNING. Returning from the coroutine will resume execution from
 * the previous yield point.
 *
 * @param ms Microseconds to sleep for.
 */
void sleep(uint16_t microseconds);

/**
 * @brief Moves the task to waiting state till given event is triggered.
 * @param id Event to wait on.
 * @see sleep()
 */
void wait_on_event(event_id_t id);

/**
 * @brief Suspends the task indefinitely.
 */
void suspend();

/**
 * @brief Check if the task was resumed from TASK_STATE_SUSPENDED.
 * @return TRUE of FALSE whether task was resumed.
 */
boolean resumed();

/**
 * @brief Check if the task was restarted.
 * @return TRUE of FALSE whether task was restarted.
 */
boolean restarted();

/**
 * @brief A task will use the function to request a resource.
 *
 * When shared flag is set, if no task owns a resource, the resource is granted
 * and shared status is set for it. But if a resource is owned by a task then
 * it will be granted only if shared status was set for it by the owner.
 *
 * When force flag is set, the task requesting a resource becomes the sole
 * owner of it and ownership of other tasks is discared even if the resource
 * has shared status set. Further requests for the resource will be denied till
 * the owner relinquishes the resource.
 *
 * @param res The resource being requested.
 * @param shared Flag whether resource should be shared.
 * @param force Flag whether to force acquisition of resource.
 * @return Zero if resource is granted, ERR_RESOURCE_DENIED otherwise.
 */
uint8_t acquire_resource(resource_id_t res, uint8_t shared, uint8_t forced);

/**
 * @brief A task will use the function to request multiple resources.
 * @param mask Bitmask of resources being requested.
 * @param shared_mask Bitmask of shared flag for each resource.
 * @param forced_mask Bitmask of forced flag for each resource.
 * @see acquire_resource()
 * @return Bitmask of resources assigned to task.
 */
uint8_t acquire_resource_mask(uint8_t mask, uint8_t shared_mask,
                           uint8_t forced_mask);
/**
 * @brief A task will use the function to relinquish a resource.
 * @param res Resource to be relinquished.
 */
void release_resource(resource_id_t res);

/**
 * @brief A task will use the function to relinquish all resource.
 */
void release_all_resources();

/**
 * @brief Check if a resource is owned by current task.
 * @param res Identifier of the resource to be checked.
 * @see acquire_resource()
 * @return 1 if resource is owned by current task, 0 otherwise.
 *
 * When an event is being handled this function will return 1.
 */
uint8_t check_resource_owner(resource_id_t res);

/**
 * @brief Check if a resource is owned by current task and return if it is not.
 * @param res Identifier of the resource to be checked.
 * @return void
 */
#define CHECK_RESOURCE_RETURN(res) \
  if (!check_resource_owner(res)) return;

/**
 * @brief Check if a resource is owned by current task and return given value
 * if it is not.
 * @param res Identifier of the resource to be checked.
 * @param val The value to be returned.
 * @return val
 */
#define CHECK_RESOURCE_RETURN_VALUE(res, val) \
  if (!check_resource_owner(res)) return val;

/**
 * @brief Get a new timer identifier if available.
 * @return INVALID_ID if new timer is unavailable, a valid timer identifier
 * otherwise.
 */
timer_id_t get_timer(void);

/**
 * @brief Relinquish a timer in use.
 * @param id Identifier of the timer to free.
 */
void discard_timer(timer_id_t id);

/**
 * @brief Set the duration for a timer, and if it should repeat.
 * @param id Identifier of the timer.
 * @param ms Duration in milliseconds.
 * @param repeat Boolean 0 or 1 denoting whether the timer should repeat or
 * should be one-shot.
 */
void set_timer_duration(timer_id_t id, uint16_t ms, boolean repeat);

/**
 * @brief Set handler callback function and data to be passed to the callback
 * function, for a timer.
 * @param id Identifier of the timer.
 * @param handler Callback function.
 * @param data Data to be passed to the callback function.
 */
void set_timer_handler(timer_id_t id, event_handler handler, task_data_t data);

/**
 * @brief Set the duration, if it should repeat or not, and the callback
 * function with its data, for a timer.
 * @param id Identifier of the timer.
 * @param ms Duration in milliseconds.
 * @param repeat Boolean 0 or 1 denoting whether the timer should repeat or
 * @param handler The callback function.
 * @param data Data to be passed to the callback function.
 * @see set_timer_duration()
 * @see set_timer_handler()
 */
#define SET_TIMER(id, ms, repeat, cb, data) \
  do { \
    set_timer_handler(id, cb, (task_data_t)data); \
    set_timer_duration(id, ms, repeat); \
  } while (0)

/**
 * @brief Start a timer.
 * @param id Identifier of the timer.
 *
 * If called for a timer that is already running, this fucntion will reset the
 * timer.
 */
void start_timer(timer_id_t id);

/**
 * @brief Stop a running timer.
 * @param id Identifier of the timer.
 *
 * If called for a timer that is not running, this function has not effect.
 */
void stop_timer(timer_id_t id);

/**
 * @brief Reset a running timer.
 * @param id Identifier of the timer.
 * @see start_timer()
 *
 * If called for a timer that is not running, this function will start the
 * timer.
 */
#define RESET_TIMER(id) start_timer(id)

/**
 * @brief Get a new event identifier if available.
 * @return The event identifier if a new event is available, INVALID_ID
 * otherwise.
 */
event_id_t register_event(void);

/**
 * @brief Relinquish an event in use.
 * @param id Identifier of the event.
 */
void deregister_event(event_id_t id);

/**
 * @brief Set a handler callback function, and the data to be passed to the
 * callback function, for an event.
 * @param id Identifier of the event.
 * @param handler Callback function.
 * @param data Data to be passed to the callback function.
 *
 * Multiple event handlers can be added for an event. The handlers will be
 * called in the order they are added.
 */
uint8_t register_event_handler(event_id_t id, event_handler handler,
    task_data_t data);

/**
 * @brief Remove an event handler for an event.
 * @param id Identifier of the event.
 * @param handler Callback function to remove.
 */
void deregister_event_handler(event_id_t id, event_handler handler);

/**
 * @brief Trigger an event instantly or defer it till next event handling
 * cycle.
 * @param id Identifier of the event.
 * @param trigger Enum denoting whether to trigger the event instantly or defer
 * it.
 * <TABLE>
 * <TR>   <TD>Values</TD>     </TR>
 * <TR>   <TD>EV_NOW</TD>     </TR>
 * <TR>   <TD>EV_DEFER</TD>   </TR>
 * </TABLE>
 */
void trigger_event(event_id_t id, event_trigger_t trigger);

/**
 * @brief Regsiter port, pin and counter value with software PWM channel and
 * start the counter.
 * @param channel Enum denoting software PWM channel.
 * @param port GPIO port.
 * @param pin GPIO pin.
 * @param value Counter top value.
 *
 * Software PWM counter starts from next systick interrupt. The counter value
 * will decide the pulse width.
 */
void register_software_pwm(pwm_channel_id_t channel, volatile uint8_t *port,
    uint8_t pin, uint8_t value);

/**
 * @brief Register pin and counter value with software PWM channel and start
 * the counter.
 * @param channel Enum denoting software PWM channel.
 * @param pin GPIO pin based on absolute pin numbering.
 * @param value Counter top value.
 * @see register_software_pwm()
 *
 * For pin numbering see port.h
 */
#define REGISTER_SOFTWARE_PWM(channel, pin, value) \
  register_software_pwm( \
      channel, \
      PIN_NUMBER_TO_PORT(pin), \
      CONVERT_PIN_NUMBER(pin), \
      value \
    )

/**
 * @brief Stop the software PWM counter.
 * @param channel Enum denoting software PWM channel.
 */
void deregister_software_pwm(pwm_channel_id_t channel);

/**
 * @brief Change the software PWM channel counter's top value affecting the
 * pulse width.
 * @param channel Enum denoting software PWM channel.
 * @param value Counter top value.
 */
void set_software_pwm_value(pwm_channel_id_t channel, uint8_t value);

static
inline
void crash(uint8_t error) __attribute__((always_inline));

void crash(uint8_t error) {
  /* get the value of program counter */
  uint16_t pc;
  asm volatile (
      "rcall getpc%=" "\n\t"
      "rjmp end%="    "\n\t"
      "getpc%=:"      "\n\t"
      "pop %B0"       "\n\t"
      "pop %A0"       "\n\t"
      "push %A0"      "\n\t"
      "push %B0"      "\n\t"
      "ret"           "\n\t"
      "end%=:"        "\n\t"
      : "=r" (pc)
      :
      : "r0"
    );

  /* write error conditions to EEPROM */
  eeprom_write_byte(EEPROM_ERROR_FLAG_ADDR, 1);
  eeprom_write_byte(EEPROM_ERROR_VALUE_ADDR, error);
  eeprom_write_byte(EEPROM_PC_VALUE_LOW_ADDR,
      (uint8_t)(pc & 0xff));
  eeprom_write_byte(EEPROM_PC_VALUE_HIGH_ADDR,
      (uint8_t)((pc >> 8) & 0xff));

  /* perform reset by watchdog timeout */
  wdt_enable(WDTO_15MS);
  asm volatile("rjmp .-2"::);
}

/** @} */

#endif /* _KERNEL_H_ */
