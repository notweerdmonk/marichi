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
 * @file kernel.c
 * @author notweerdmonk
 * @brief implements the kernel and its services
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/

#include <kernel_config.h>
#include <kernel.h>
#include <setjmp.h>
#include <opt/debugger/debugger.h>
#include <uart.h>
#include <lcd.h>
#include <adc.h>

/* TODO: use basic types instead of typedefs, use uint8_t as descriptor type */
/* TODO: return value of setjmp, on fake and actual returns */
/* TODO: move setjmp and longjmp to assembly source files */
/* TODO: change return type of task_cb from (void*) to (uint16_t*) */
/* TODO: write wiki */
/* TODO: tutorial in wiki for coroutines */
/* TODO: tutorial in wiki for debugger */
/* TODO: add all stubs and and add stub warnings */
/* TODO: make some macros static functions */
/* TODO: enable/disable software pwm channels */
/* TODO: software PWM resource control */
/* TODO: ability to trigger debugger on exception? */
/* TODO: create an archive and link it with user code */
/* TODO: use memblock */
/* TODO: implement priotiy queue for tasks */
/* TODO: use separate assembly function for get_pc */
/* TODO: save and restore SREG in setjmp/longjmp */
/* TODO: mark resources shareable or otherwise */
/* TODO: add eeprom module */
/* TODO: gpio resource control */
/* TODO: add a lean mode just to run one task without the kernel */
/* TODO: revisit and homogenise function and macro names */
/* TODO: improve Makefile */
/* TODO: lint with cppcheck */
/* TODO: freeze timers when task is suspended? */
/* TODO: if timer fires when task is suspened, how to handle it? */
/* TODO: event to restart tasks? */
/* TODO: Use Sphinx to genereate Doxygen documentation for Read the Docs */

/*
 * NOTE
 * Using integer indices to access arrays intead of pointers generates lesser
 * instructions.
 */
/*****************************************************************************/
/* Declarations                                                              */
/*****************************************************************************/

/*
 * Resources
 */
#if c_SYS_MAX_TASKS < 7

#define RESOURCE_SHARED_MASK 0x80
#define RESOURCE_FORCED_MASK 0x40
#define RESOURCE_OWNERS_MASK 0x3f

typedef struct _resource_info {
  uint8_t status;
  uint8_t prev_status;
} resource_info_t;

#elif c_SYS_MAX_TASKS > 6 && c_SYS_MAX_TASKS < 15

#define RESOURCE_SHARED_MASK 0x8000
#define RESOURCE_FORCED_MASK 0x4000
#define RESOURCE_OWNERS_MASK 0x3fff

typedef struct _resource_info {
  uint16_t status;
  uint16_t prev_status;
} resource_info_t;

#else
#error c_SYS_MAX_TASKS is too large
#endif

/*****************************************************************************/

/*
 * Tasks
 */
typedef enum _task_state {
  TASK_STATE_RUNNING = 0,
  TASK_STATE_SUSPENDED,
  TASK_STATE_WAITING,
  TASK_STATE_SLEEPING
} task_state_t;

typedef enum _signal {
  TASK_SIG_SUSPEND,
  TASK_SIG_RUN,
  TASK_SIG_RESTART,
  TASK_SIG_WAIT,
  TASK_SIG_SLEEP,
  TASK_SIG_RESUME
} signal_t;

#define IS_SERVICE_MASK     1
#define WAS_RESTARTED_MASK  2
#define WAS_RESUMED_MASK    4

typedef struct _task {
  /* task state */
  task_state_t prev_state : 2;
  task_state_t state : 2;

  /* task status */
  uint8_t status : 4;

  /* callback and data */
  task_handler handler;
  task_data_t data;

  /* coroutine state */
  uint16_t *p_cr_state;
} task_t;

#if c_SYS_MAX_TASKS < 7
typedef uint8_t task_mask_t;
#elif c_SYS_MAX_TASKS > 6 && c_SYS_MAX_TASKS < 15
typedef uint16_t task_mask_t;
#else
#error c_SYS_MAX_TASKS is too large
#endif

typedef struct _tasks_info {
  /* list of tasks */
  task_t tasks[c_SYS_MAX_TASKS];

  /* current task id */
  task_id_t cur_id;

  /* bitmask from cur_id */
  task_mask_t cur_mask;

  /* highest id */
  task_id_t max_id;

} tasks_info_t;

/*****************************************************************************/

/*
 * Events
 */
typedef struct _event {
  /* lists of callbacks and data */
  event_handler handlers[c_SYS_MAX_EVT_HANDLERS];
  task_data_t data[c_SYS_MAX_EVT_HANDLERS];

  /* task id, used to deregister events when task is deregistered */
  task_id_t task_id;

  /* highest id */
  task_id_t max_id;
} event_t;

typedef struct _events_info {
  /* list of events */
  event_t events[c_SYS_MAX_EVENTS];

  /* bit mask for event registration */
  volatile uint8_t ids;

  /* bit mask for triggered events */
  volatile uint8_t triggered;
} events_info_t;

/*****************************************************************************/

/*
 * Software timers
 */
typedef struct _timer {
  /* callback and data */
  volatile event_handler handler;
  task_data_t data;

  /* task id */
  task_id_t task_id;

  /* down counter */
  uint16_t counter;

  /* top value */
  volatile uint16_t top;
} timer_t;

/*
 * NOTE:
 * Use a custom type for timers mask. The width of the bitmask is variable based
 * on the maximum number of timers. Defining a custom type facilitates
 * declaration of mask variables without accounting for the bit-width.
 */
#if c_SYS_MAX_SW_TIMERS < 9

typedef uint8_t timer_mask_t;

#elif c_SYS_MAX_SW_TIMERS < 17

typedef uint16_t timer_mask_t;

#elif c_SYS_MAX_SW_TIMERS < 33

typedef uint32_t timer_mask_t;

#endif

typedef struct _timers_info {
  /* list of timers */
  timer_t timers[c_SYS_MAX_SW_TIMERS];

  /* bit mask for timers which repeat */
  volatile timer_mask_t repeat;

  /* bit mask for timers which have fired */
  volatile timer_mask_t fired;
} timers_info_t;

/*****************************************************************************/

/*
 * Software PWM
 */
typedef struct _pwm_channel {
  /* PWM port and pin */
  volatile uint8_t *port;
  volatile uint8_t pin;

  /* counter */
  volatile uint8_t count;

  /* top value */
  volatile uint8_t top;
} pwm_channel_t;

typedef struct _pwm {
  pwm_channel_t channels[2];
} pwm_t;

/*****************************************************************************/
/* Data                                                                      */
/*****************************************************************************/

/*
 * Systick counter
 */
static uint16_t sys_ticks;

static timers_info_t software_timers;

static events_info_t events_info;

static pwm_t software_pwm;

static tasks_info_t tasks_info;

static resource_info_t resources[SYS_RES_MAX];

static jump_buf_t task_reset_jump_buf;

static trap_t trap_event;

#ifndef PORT_WDT_VECT

static volatile uint16_t task_time_counter;

static uint16_t task_timeout;

#endif

/*****************************************************************************/
/* Forward declarations                                                      */
/*****************************************************************************/

/*
 * Tasks
 */
#define TASK(id) tasks_info.tasks[id]

#define CUR_TASK tasks_info.tasks[tasks_info.cur_id]

#define CUR_TASK_ID tasks_info.cur_id

#define CUR_TASK_MASK tasks_info.cur_mask

static
inline
task_state_t run_task(task_t *p_task) __attribute__((always_inline));

/*
 * Resources
 */
#define IS_RESOURCE_SHARED(status) \
  (status & RESOURCE_SHARED_MASK)

#define IS_RESOURCE_FORCED(status) \
  (status & RESOURCE_FORCED_MASK)

static
void release_resource_internal(uint8_t task_mask, resource_id_t res);

/*****************************************************************************/
/* Expose task reset jump buffer                                             */
/*****************************************************************************/

const jump_buf_t* kernel_task_reset_jump_buffer() {
  return &task_reset_jump_buf;
}

/*****************************************************************************/
/* Nonlocal goto                                                             */
/*****************************************************************************/

/**
 * Transfer control from one function to a predetermined location in another
 * function.
 */

/**
 * setjmp is used to mark the point to which control will be transferred. This
 * contains the information about the execution context (the environment of the
 * calling function).
 *
 * typedef struct _jump_buf {
 *   uint8_t buf[38];
 * } jump_buf_t[1];
 */
void setjmp(jump_buf_t buf) {
  asm volatile(
      "st Z, r0"                 "\n\t"
      "std Z+1, r1"              "\n\t"
      "std Z+2, r2"              "\n\t"
      "std Z+3, r3"              "\n\t"
      "std Z+4, r4"              "\n\t"
      "std Z+5, r5"              "\n\t"
      "std Z+6, r6"              "\n\t"
      "std Z+7, r7"              "\n\t"
      "std Z+8, r8"              "\n\t"
      "std Z+9, r9"              "\n\t"
      "std Z+10, r10"            "\n\t"
      "std Z+11, r11"            "\n\t"
      "std Z+12, r12"            "\n\t"
      "std Z+13, r13"            "\n\t"
      "std Z+14, r14"            "\n\t"
      "std Z+14, r15"            "\n\t"
      "std Z+16, r16"            "\n\t"
      "std Z+17, r17"            "\n\t"
      "std Z+18, r18"            "\n\t"
      "std Z+19, r19"            "\n\t"
      "std Z+20, r20"            "\n\t"
      "std Z+21, r21"            "\n\t"
      "std Z+22, r22"            "\n\t"
      "std Z+23, r23"            "\n\t"
      "std Z+24, r24"            "\n\t"
      "std Z+25, r25"            "\n\t"
      "std Z+26, r26"            "\n\t"
      "std Z+27, r27"            "\n\t"
      "std Z+28, r28"            "\n\t"
      "std Z+29, r29"            "\n\t"
      "mov __tmp_reg__, r30"     "\n\t"
      "std Z+30, __tmp_reg__"    "\n\t"
      "mov __tmp_reg__, r31"     "\n\t"
      "std Z+31, __tmp_reg__"    "\n\t"
      "in __tmp_reg__, __SP_H__" "\n\t"
      "std Z+32, __tmp_reg__"    "\n\t"
      "in __tmp_reg__, __SP_L__" "\n\t"
      "std Z+33, __tmp_reg__"    "\n\t"
      "rcall getpc%="            "\n\t"
      "rjmp end%="               "\n\t"
      "getpc%=:"                 "\n\t"
      "pop r16"                  "\n\t"
      "pop r17"                  "\n\t"
      "std Z+34, r16"            "\n\t" /* MSB of PC */
      "std Z+35, r17"            "\n\t" /* LSB of PC */
      "push r17"                 "\n\t"
      "push r16"                 "\n\t"
      "ret"                      "\n\t"
      "end%=:"                   "\n\t"
      :
      : "z" (buf)
      : "r0", "r16", "r17"
    );
}

/**
 * longjmp restores the stack and cpu registers to the state at the time of the
 * corresponding setjmp call. Execution will continue from the point where
 * setjmp returns.
 */
void longjmp(jump_buf_t buf) {
  LONGJMP(buf);
}

/*****************************************************************************/
/* Boot                                                                      */
/*****************************************************************************/

/* Diagnostics */

extern uint8_t get_atboot_mcusr();

/*****************************************************************************/
/* Reset a task                                                              */
/*****************************************************************************/

void kernel_reset_task() {
  LONGJMP(task_reset_jump_buf);
}

/*****************************************************************************/
/* Trap                                                                      */
/*****************************************************************************/

/*
 * Trap catches asynchronous events.
 */

static
void init_trap() {
  /* low level of trap pin generates interrupt */
  OUTPUT_PIN_IMM(PORT_TRAP_PORT, PORT_TRAP_PIN);
  SET_PIN_IMM(PORT_TRAP_PORT, PORT_TRAP_PIN);

  /* enable trap interrupt */
  PORT_TRAP_INTERRUPT_MASK_REGISTER = \
    PORT_TRAP_INTERRUPT_MASK;

  trap_event = TRAP_TYPE_NONE;
}

ISR(PORT_TRAP_VECT) {
  /* restore TRAP_PIN to high level */
  SET_PIN_IMM(PORT_TRAP_PORT, PORT_TRAP_PIN);

  /* handle trap */
  switch (trap_event) {
    case TRAP_TYPE_TASK_TIMEOUT:
      {
        /* reset wdt */
        wdt_reset();

        /* release any resources owned by current task */
        for (uint8_t i = 0; i < SYS_RES_MAX; ++i) {
          release_resource_internal(CUR_TASK_MASK, i);
        }

        LONGJMP(task_reset_jump_buf);
        asm("reti");
      }

#ifdef __ENABLE_RESMGMT
    case TRAP_TYPE_RES_CHK:
      {
#ifdef GPIOR3
        resource_id_t res = (resource_id_t)GPIOR3;
        GPIOR3 = (CUR_TASK_ID < INVALID_ID) ? \
               resources[res].status & CUR_TASK_MASK : 1;
#else
        resource_id_t res;
        asm volatile (
            "mov %0, r24"   "\n\t"
            : "=r" (res)
            :
            : "r24"
        );
        asm volatile (
            "mov r24, %0"   "\n\t"
            :
            : "r" ((CUR_TASK_ID < INVALID_ID) ? \
              resources[res].status & CUR_TASK_MASK : 1)
            : "r24"
        );
#endif
      }
#endif /* __ENABLE_RESMGMT */

    default:
      ;
  }
}

/*
 * trap() function is used to trap an trappable event
 */
void trap(trap_t t) {
  trap_event = t;

  /* pull TRAP_PIN low */
  CLR_PIN_IMM(PORT_TRAP_PORT, PORT_TRAP_PIN);
}

/*****************************************************************************/
/* Task state machine                                                        */
/*****************************************************************************/

#define SET_TASK_STATE(p_task, new_state) \
  p_task->prev_state = p_task->state, \
  p_task->state = new_state \

#define RESTORE_TASK_STATE(p_task) \
  p_task->state = p_task->prev_state

static
void change_task_state(task_t *p_task, signal_t sig) {
  if (p_task) {
    task_state_t cur_state = p_task->state;
    task_state_t next_state = INVALID_ID;

    /*
     * NOTE:
     * Using switch-case generates lesser assembly code than if-else
     */
    switch (sig) {
      case TASK_SIG_SUSPEND:
        switch (cur_state) {
          case TASK_STATE_RUNNING:
          case TASK_STATE_WAITING:
          case TASK_STATE_SLEEPING:
            next_state = TASK_STATE_SUSPENDED;
            break;

          default:
            ;
        }
        break;

      case TASK_SIG_RUN:
        /*
         * cur_state != TASK_STATE_WAITING &&
         * cur_state != TASK_STATE_SLEEPING
         */
        if (cur_state <= TASK_STATE_SUSPENDED) {
          break;
        }
        /* fall-through */
      case TASK_SIG_RESTART:
        /*
         * cur_state != TASK_STATE_SUSPENDED &&
         * cur_state != TASK_STATE_WAITING &&
         * cur_state != TASK_STATE_SLEEPING
         */
        if (cur_state == TASK_STATE_RUNNING) {
          break;
        }
        next_state = TASK_STATE_RUNNING;
        break;

      case TASK_SIG_WAIT:
        if (cur_state == TASK_STATE_RUNNING) {
          next_state = TASK_STATE_WAITING;
        }
        break;

      case TASK_SIG_SLEEP:
        if (cur_state == TASK_STATE_RUNNING) {
          next_state = TASK_STATE_SLEEPING;
        }
        break;

      case TASK_SIG_RESUME:
        if (cur_state == TASK_STATE_SUSPENDED) {
          next_state = p_task->prev_state;
        }
        break;
    }

    if (next_state != INVALID_ID) {
      p_task->prev_state = p_task->state;
      p_task->state = next_state;
    }
  }
}

/*****************************************************************************/
/* Tasks                                                                     */
/*****************************************************************************/

#define COMPARE_TASK_DATA(data1, data2) \
  ( data1.id == data2.id || \
    data1.n == data2.n || \
    data1.u == data2.u || \
    data1.ptr == data2.ptr )

static
void pick_next_task() {
  do {
    if (++CUR_TASK_ID == tasks_info.max_id) {
      CUR_TASK_ID = 0;
    }
    CUR_TASK_MASK = BITMASK(CUR_TASK_ID);
  } while (CUR_TASK.handler == NULL);
}

/*
 * Carry out current task
 */
static
inline
void do_task() {
  task_t *p_task = &CUR_TASK;

  if (p_task->state == TASK_STATE_RUNNING) {

    p_task->p_cr_state = p_task->handler(p_task->data);

    CLR_MASK(p_task->status, WAS_RESUMED_MASK);
    CLR_MASK(p_task->status, WAS_RESTARTED_MASK);
  }
}

/*
 * Suspend a task
 */
static
inline
__attribute__((always_inline))
task_state_t suspend_task(task_t *p_task) {
  if (p_task) {
    change_task_state(p_task, TASK_SIG_SUSPEND);
  }
  return p_task->state;
}

/*
 * Run a task that is waiting or sleeping
 */
static
inline
__attribute__((always_inline))
task_state_t run_task(task_t *p_task) {
  if (p_task) {
    change_task_state(p_task, TASK_SIG_RUN);
  }
  return p_task->state;
}

/*
 * Resume a task
 */
static
inline
__attribute__((always_inline))
task_state_t resume_task(task_t *p_task) {
  if (p_task) {
    change_task_state(p_task, TASK_SIG_RESUME);
    SET_MASK(p_task->status, WAS_RESUMED_MASK);
  }
  return p_task->state;
}

static
inline
__attribute__((always_inline))
task_state_t restart_task(task_t *p_task) {
  if (p_task) {
    if (p_task->p_cr_state) *(p_task->p_cr_state) = 0;
    change_task_state(p_task, TASK_SIG_RESTART);
    SET_MASK(p_task->status, WAS_RESTARTED_MASK);
  }
  return p_task->state;
}

/*
 * Suspend apps sharing given resource
 */
UNUSED_FUNCTION
static
void suspend_shared_tasks(resource_id_t res) {
  resource_info_t *p_res = &resources[res];
  if (IS_RESOURCE_SHARED(p_res->status)) {
    for (task_id_t task_id = 0; task_id < tasks_info.max_id; task_id++) {
      if (p_res->status & (1<<task_id)) {
        suspend_task(&TASK(task_id));
      }
    }
  }
}

/*
 * Resume apps sharing given resource
 */
UNUSED_FUNCTION
static
void resume_shared_tasks(resource_id_t res) {
  resource_info_t *p_res = &resources[res];
  if (IS_RESOURCE_SHARED(p_res->status)) {
    for (task_id_t task_id = 0; task_id < tasks_info.max_id; task_id++) {
      if (p_res->status & (1<<task_id)) {
        resume_task(&TASK(task_id));
      }
    }
  }
}

/*
 * APIs
 *
 * These functions are only restricted to the debugger module.
 */

void suspend_all_apps() {
  for (task_id_t task_id = 0; task_id < tasks_info.max_id; task_id++) {
    if (TASK(task_id).handler) {
      if (!(TASK(task_id).status & IS_SERVICE_MASK)) {
        suspend_task(&TASK(task_id));
      }
    }
  }
}

void resume_all_apps() {
  for (task_id_t task_id = 0; task_id < tasks_info.max_id; task_id++) {
    if (TASK(task_id).handler) {
      if (!(TASK(task_id).status & IS_SERVICE_MASK)) {
        resume_task(&TASK(task_id));
      }
    }
  }
}

void restart_all_apps() {
  for (task_id_t task_id = 0; task_id < tasks_info.max_id; task_id++) {
    if (TASK(task_id).handler) {
      if (!(TASK(task_id).status & IS_SERVICE_MASK)) {
        restart_task(&TASK(task_id));
      }
    }
  }
}

/******************************************************************************/
/* Task policing                                                              */
/******************************************************************************/

/*
 * Watchdog timer interrupt is used for policing tasks on newer devices.
 * @see kernel/wdt.c
 *
 * Watchdog timer does not have an interrupt on older devices. A dedicated timer
 * is used to check if task is stuck.
 *
 * The basic idea is to count microseconds per task and bail out tasks which are
 * frozen.
 *
 * Task premption and context switching are not available.
 */

#ifdef PORT_WDT_VECT

static
inline
void wdt_enable_interrupt(const uint8_t timeout) {
  PORT_WDT_ENABLE_INTERRUPT(timeout);
}

#else

static
void init_task_timeout(uint8_t timeout) {
  task_timeout = PORT_GET_TASK_TIMEOUT(timeout)
}

static
void reset_task_timer() {
  cli();
  task_time_counter = 0;
  sei();
}

static
void tick_task_timer() {
  if ((sys_ticks % c_SYS_MS_TICKS) == 0) {
    if (++task_time_counter == task_timeout) {
      trap(TRAP_TYPE_TASK_TIMEOUT);
    }
  }
}

#endif

/*****************************************************************************/
/* Software timers                                                           */
/*****************************************************************************/

#define SOFTWARE_TIMER(id) software_timers.timers[id]

static
void tick_software_timers() {
  if ((sys_ticks % c_SYS_MS_TICKS) == 0) {
    timer_mask_t repeat = software_timers.repeat;
    timer_mask_t fired = 1;

    for (uint8_t id = 0; id < c_SYS_MAX_SW_TIMERS; ++id) {
      timer_t *p_timer = &software_timers.timers[id];

      /* handle active timers */
      if (p_timer->handler) {
        if (p_timer->counter > 0) {
          if (--p_timer->counter == 0) {
            SET_MASK(software_timers.fired, fired);
            if (repeat & 1) {
              p_timer->counter = p_timer->top;
            }
          }
        }
      }

      repeat = repeat >> 1;
      fired = fired << 1;
    }
  }
}

static
void fire_software_timers() {
  timer_mask_t fired_mask = software_timers.fired;
  timer_mask_t fired = 1;

  for (uint8_t id = 0; id < c_SYS_MAX_SW_TIMERS; ++id) {
    timer_t *p_timer = &software_timers.timers[id];

    if (fired_mask & 1) {
      if (p_timer->handler) {
        (*(p_timer->handler))(id, p_timer->data);
        CLR_MASK(software_timers.fired, fired);
      }
    }

    fired_mask = fired_mask >> 1;
    fired = fired << 1;
  }
}

/*
 * APIs
 */

timer_id_t get_timer(void) {
  for(uint8_t id = 0; id < c_SYS_MAX_SW_TIMERS; id++) {
    if (SOFTWARE_TIMER(id).handler == NULL) {
      SOFTWARE_TIMER(id).task_id = CUR_TASK_ID;
      return id;
    }
  }
  return INVALID_ID;
}

void discard_timer(timer_id_t id) {
  if (id < c_SYS_MAX_SW_TIMERS) {
    SOFTWARE_TIMER(id).handler = NULL;
  }
}

void set_timer_duration(timer_id_t id, uint16_t ms, boolean repeat) {
  if (id < c_SYS_MAX_SW_TIMERS) {
    SOFTWARE_TIMER(id).top = ms;
    uint8_t mask = BITMASK(id);
    if (repeat)
      SET_MASK(software_timers.repeat, mask);
    else
      CLR_MASK(software_timers.repeat, mask);
  }
}

void set_timer_handler(timer_id_t id, timer_handler handler, task_data_t data) {
  if (id < c_SYS_MAX_SW_TIMERS) {
    timer_t *p_timer = &SOFTWARE_TIMER(id);
    if (handler != NULL) {
      p_timer->handler = handler;
      p_timer->data = data;
    }
  }
}

void start_timer(timer_id_t id) {
  if (id < c_SYS_MAX_SW_TIMERS) {
    SOFTWARE_TIMER(id).counter = SOFTWARE_TIMER(id).top;
  }
}

void stop_timer(timer_id_t id) {
  if (id < c_SYS_MAX_SW_TIMERS) {
    SOFTWARE_TIMER(id).counter = 0;
  }
}

/*****************************************************************************/
/* Events                                                                    */
/*****************************************************************************/

static
void handle_events() {
  uint8_t active = events_info.ids;
  uint8_t triggered_events = events_info.triggered;
  uint8_t mask = 1;

  /*
   * Setting CUR_TASK_ID to INVALID_ID will cause check_resource_owner to return
   * non-zero
   */
  task_mask_t task_mask = CUR_TASK_MASK;
  CUR_TASK_MASK = RESOURCE_OWNERS_MASK;

  for (uint8_t id = 0; id < c_SYS_MAX_EVENTS; ++id) {
    event_t *p_event = &events_info.events[id];

    if ((active & 1) && (triggered_events & 1)) {
      for (uint8_t hid = 0; hid < p_event->max_id; hid++) {
        if (p_event->handlers[hid]) {
          (*(p_event->handlers[hid]))(id, p_event->data[hid]);
        }
      }

      CLR_MASK(events_info.triggered, mask);
    }

    active = active >> 1;
    triggered_events = triggered_events >> 1;
    mask = mask << 1;
  }

  CUR_TASK_MASK = task_mask;
}

/*
 * APIs
 */

event_id_t register_event(void) {
  uint8_t id;
  uint8_t event_ids = events_info.ids;
  uint8_t mask = 1;

  for (id = 0; id < c_SYS_MAX_EVENTS; ++id) {
    if (!(event_ids & 1)) {
      SET_MASK(events_info.ids, mask);
      events_info.events[id].task_id = CUR_TASK_ID;
      return id;
    }

    event_ids = event_ids >> 1;
    mask = mask << 1;
  }
  return INVALID_ID;
}

void deregister_event(event_id_t id) {
  if (id < c_SYS_MAX_EVENTS) {
    CLR_BIT(events_info.ids, id);
  }
}

uint8_t register_event_handler(event_id_t id, event_handler handler,
    task_data_t data) {

  uint8_t ret = ERR_OUT_OF_MEMORY;

  if (id >= c_SYS_MAX_EVENTS) {
    return ERR_INVALID_ARGUMENT;
  }

  event_t *p_event = &events_info.events[id];

  if (handler == NULL) {
    return ERR_INVALID_ARGUMENT;
  }

  for (uint8_t hid = 0; hid < c_SYS_MAX_EVT_HANDLERS; hid++) {

    if (p_event->handlers[hid] == NULL) {

      p_event->handlers[hid] = handler;
      p_event->data[hid] = data;

      if (hid >= p_event->max_id) {
        p_event->max_id++;
      }

      ret = 0;
      break;

    } else if (p_event->handlers[hid] == handler &&
        COMPARE_TASK_DATA(p_event->data[hid], data)) {

      ret = 0;
      break;
    }
  }

  return ret;
}

void deregister_event_handler(event_id_t id, event_handler handler) {
  if (id >= c_SYS_MAX_EVENTS) {
    return;
  }
  event_t *p_event = &events_info.events[id];

  if (handler == NULL) {
    return;
  }

  for (uint8_t hid = 0; hid < c_SYS_MAX_EVT_HANDLERS; hid++) {
    if (p_event->handlers[hid] == handler) {
      p_event->handlers[hid] = NULL;
      p_event->data[hid].ptr = NULL;
      return;
    }
  }
}

void trigger_event(event_id_t id, event_trigger_t trigger) {
  if (id >= c_SYS_MAX_EVENTS) {
    return;
  }

  if (trigger == EV_DEFER) {
    SET_MASK(events_info.triggered, BITMASK(id));
    return;
  }

  event_t *p_event = &events_info.events[id];

  /*
   * Setting CUR_TASK_ID to INVALID_ID will cause check_resource_owner to
   * return non-zero
   */
  task_mask_t task_mask = CUR_TASK_MASK;
  CUR_TASK_MASK = RESOURCE_OWNERS_MASK;

  for (uint8_t hid = 0; hid < p_event->max_id; hid++) {
    if (p_event->handlers[hid]) {
      (*(p_event->handlers[hid]))(id, p_event->data[hid]);
    }
  }

  CUR_TASK_MASK = task_mask;
}

/*****************************************************************************/
/* Software PWM                                                              */
/*****************************************************************************/

static
void tick_software_pwm() {
  for (uint8_t c = 0; c < PWM_CHANNEL_B + 1; c++) {
    pwm_channel_t *p_channel = &software_pwm.channels[c];

    if (((int)p_channel->port) != 0) {

      if ((++p_channel->count > p_channel->top) &&
          (~(*(p_channel->port)) & (1<<p_channel->pin))) {

        SET_BIT(*(p_channel->port), p_channel->pin);
      }
      else if ((p_channel->count < p_channel->top) &&
               (*(p_channel->port) & (1<<p_channel->pin))) {

        CLR_BIT(*(p_channel->port), p_channel->pin);
      }
    }
  }
}

/*
 * APIs
 */

void register_software_pwm(pwm_channel_id_t channel, volatile uint8_t *port,
                           uint8_t pin, uint8_t value) {
  if ( (port != NULL) && (pin < 8) ) {

#if c_SYS_PWM_MAX < __UINT8_MAX__
    if (value > c_SYS_PWM_MAX) {
      value = c_SYS_PWM_MAX;
    }
#endif

    /* execute atomically */
    cli();

    software_pwm.channels[channel].port = port;
    software_pwm.channels[channel].pin = pin;
    software_pwm.channels[channel].count = 0;
    software_pwm.channels[channel].top = c_SYS_PWM_MAX - value;

    SET_BIT(
        *(software_pwm.channels[channel].port - 1),
        software_pwm.channels[channel].pin
      );
    CLR_BIT(
        *software_pwm.channels[channel].port,
        software_pwm.channels[channel].pin
      );

    sei();
  }
}

void deregister_software_pwm(pwm_channel_id_t channel) {
  cli();
  software_pwm.channels[channel].port = NULL;
  sei();
}

void set_software_pwm_value(pwm_channel_id_t channel, uint8_t value) {
#if c_SYS_PWM_MAX < __UINT8_MAX__
  if (value > c_SYS_PWM_MAX) {
    value = c_SYS_PWM_MAX;
  }
#endif

  /* execute atomically */
  cli();
  software_pwm.channels[channel].top = c_SYS_PWM_MAX - value;
  sei();
}
/*****************************************************************************/
/* Systick                                                                   */
/*****************************************************************************/

static
void tick_sys_timer() {
  if (++sys_ticks > c_SYS_MAX_TICKS) {
    sys_ticks = 1;
  }
}

/* Timer interrupt routine */
ISR(PORT_SYS_TICK_VECT) {

  /* system tick */
  tick_sys_timer();

#ifndef PORT_WDT_VECT
  /* task timer */
  tick_task_timer();
#endif

  /* software timers */
  tick_software_timers();

  /* software pwm */
  tick_software_pwm();
}

/*****************************************************************************/
/* Services                                                                  */
/*****************************************************************************/

#ifdef __ENABLE_DEBUGGER

debugger_params_t debugger_params = { 0, };

const char debugger_start_msg[] PROGMEM =
  "Debugger is running"NEWLINE_STRING;

const char debugger_stop_msg[] PROGMEM =
  NEWLINE_STRING"Debugger has stopped"NEWLINE_STRING;

/*
 * Callback function passed to UART, used to register debugger
 */
void toggle_debugger(UNUSED_VARIABLE event_id_t ev_id,
                     UNUSED_VARIABLE task_data_t data) {
  static task_id_t dbg_id = INVALID_ID;

  if (dbg_id == INVALID_ID) {

#if VERBOSITY >= VERBOSE1
    debugger_params.verbosity = 1;
#endif

    dbg_id = register_task(
        &debugger_task,
        (task_data_t)(void*)&debugger_params,
        TRUE
      );

    uart_clear();
    uart_put_pgm_string(debugger_start_msg);

  } else {
    deregister_task(dbg_id);
    dbg_id = INVALID_ID;

    uart_put_pgm_string(debugger_stop_msg);
  }
}

#endif /* __ENABLE_DEBUGGER */

/*
 * Initialize kernel services
 */
static
void init_services() {

#ifdef __ENABLE_UART
  uart_init(
      &(uart_config_t){
        .baud_rate = 0,
        .char_size = 8,
        .stop_bits = 1,
        .parity = UART_PARITY_DISABLED
      }
    );
#endif

#ifdef __ENABLE_LCD
  lcd_set_pins(
      &(lcd_data_t){
        .rs = 0,
        .en = 1,
        .rw = PIN_NC,
        .bl = 2,
        .D0 = PIN_NC,
        .D1 = PIN_NC,
        .D2 = PIN_NC,
        .D3 = PIN_NC,
        .D4 = 19,
        .D5 = 20,
        .D6 = 21,
        .D7 = 22,
        .data_port = 2
      }
    );

  if (register_task(&lcd_task, (task_data_t)NULL, TRUE) == INVALID_ID) {
    crash(ERR_INVALID_RETURN);
  }
#endif

#ifdef __ENABLE_DEBUGGER
  if (uart_register_match(
        "\x1B[17~", /* F6 */
        &toggle_debugger,
        (task_data_t)NULL
      )) {
    crash(ERR_INVALID_RETURN);
  }
#endif
}

/*****************************************************************************/
/* Power reduction                                                           */
/*****************************************************************************/

void set_power_reduction(resource_id_t res) {
  if (res < SYS_RES_MAX) {
    switch (res) {
      case SYS_RES_TIMER0:
        PORT_SET_PWR_REDUCTION_TIMER0();
        break;
      case SYS_RES_TIMER1:
        PORT_SET_PWR_REDUCTION_TIMER1();
        break;
      case SYS_RES_TIMER2:
        PORT_SET_PWR_REDUCTION_TIMER2();
        break;
      case SYS_RES_UART:
        PORT_SET_PWR_REDUCTION_USART0();
        break;
      case SYS_RES_ADC:
        PORT_SET_PWR_REDUCTION_ADC();
        break;
      case SYS_RES_SPI:
        PORT_SET_PWR_REDUCTION_SPI();
        break;
      case SYS_RES_TWI:
        PORT_SET_PWR_REDUCTION_TWI();
        break;
      default:
        ;
    }
  }
}

/*****************************************************************************/
/* Kernel                                                                    */
/*****************************************************************************/

#if VERBOSITY >= VERBOSE1
extern void print_boot_msg();
#endif

#ifdef __STANDALONE
static
#endif
void print_messages() {
  uart_clear();

#if VERBOSITY >= VERBOSE1
  print_boot_msg();
#endif

#ifdef __PRINT_MCUSR
  char hex[3];
  byte_to_hex(get_atboot_mcusr(), hex);
  uart_put_string("MCUSR=");
  uart_put_string(hex);
  uart_new_line();
  uart_new_line();
#endif

#ifdef __ENABLE_DEBUGGER
  static const char debugger_enable_prompt[] PROGMEM =
    "Press 'F6' for debugger"NEWLINE_STRING;
  uart_put_pgm_string(debugger_enable_prompt);
#endif
}

/*****************************************************************************/

/**
 * @brief Initialize the kernel.
 * @param systick_freq Frequency of system tick.
 */
#ifdef __STANDALONE
static
#endif
void init_kernel(unsigned long systick_freq) {
  /* Configure one of the timers for system tick. */
  /* 0.1 millisecond tick */
  PORT_INIT_SYSTICK_TIMER(systick_freq);

  /* Enable global interrupts. */
  sei();

  /* Initialize peripherals and services like debugger. */
  CUR_TASK_ID = INVALID_ID;
  CUR_TASK_MASK = RESOURCE_OWNERS_MASK;
  for (uint8_t i = 0; i < SYS_RES_MAX; i++) {
    resources[i].status = RESOURCE_OWNERS_MASK;
  }

  /* Initialize trap service. */
  init_trap();

  /* Initialize peripheral services. */
  init_services();

  for (uint8_t i = 0; i < SYS_RES_MAX; i++) {
    resources[i].status = 0;
  }

}

/**
 * @brief Run the kernel.
 *
 * This function will not return.
 */
#ifdef __STANDALONE
static
#endif
__attribute__((noreturn))
void run_kernel() {
  /* set sleep mode to idle */
  set_sleep_mode(SLEEP_MODE_IDLE);

  /*
   * Enable watchdog timer and/or task timer.
   *
   * Choose a value for the timeout based on the longest of the durations
   * required by each application. Define c_SYS_WDT_TIMEOUT in file
   * kernel_config.h.
   */

#ifdef PORT_WDT_VECT

  /* enable watchdog timer interrupt */
  wdt_enable_interrupt(c_SYS_WDT_TIMEOUT);

#else

  /* enable watchdog timer for system reset */
  wdt_enable(c_SYS_WDT_TIMEOUT);

  /* initialize task timer */
  init_task_timeout(c_SYS_WDT_TIMEOUT);
#endif


  for(;;) {
    /*
     * NOTE:
     * !CAUTION!
     * Entering Idle sleep mode will start a conversion if ADC is enabled.
     */
    if (!(ADC_IS_ENABLED() && SLEEP_MODE_IS_IDLE())) {
      /* zzz */
      sleep_mode();
    }

    /* resume execution after systick interrupt */

    /* call timer handlers for timers that have fired */
    fire_software_timers();

    /* call handlers for events that have triggered */
    handle_events();

    /* set jump point */
    setjmp(task_reset_jump_buf);

    /* move to next the one */
    pick_next_task();

    /* perform current task */
    do_task();

#ifndef PORT_WDT_VECT
    /* reset task timer after handling a task */
    reset_task_timer();
#endif

    /* reset watchdog timer */
    wdt_reset();
  }
}

#ifdef __STANDALONE
__attribute__((constructor))
void pre_main() {
  /* Initialization */
  init_kernel(F_CPU/F_SYS_TICK);

  /* Print boot messages over UART */
  print_messages();

}

__attribute__((destructor))
void post_main() {
  /*
   * Run the kernel
   * This function does not return
   */
  run_kernel();
}

#endif /* __STANDALONE */
/*****************************************************************************/

__attribute__((weak)) void tasks_main() {
}

/*****************************************************************************/
/*
 * Release resource for a task
 */
void release_resource_internal(uint8_t task_mask, resource_id_t res) {
  resource_info_t *p_res = &resources[res];

  if (p_res->status & task_mask) {
    if (IS_RESOURCE_FORCED(p_res->status)) {
      p_res->status = p_res->prev_status;
    }
    else {
      CLR_MASK(p_res->status, task_mask);
      if (!(p_res->status & RESOURCE_OWNERS_MASK)) {
        CLR_MASK(p_res->status, RESOURCE_SHARED_MASK);
      }
    }
  }
}

/*****************************************************************************/

/*
 * APIs
 */

task_id_t register_task(task_handler handler, task_data_t data,
                        boolean is_service) {
  uint8_t id;
  task_t *p_task = tasks_info.tasks;

  if (handler != NULL) {
    for (id = 0; id < c_SYS_MAX_TASKS; ++id, ++p_task) {
      if (p_task->handler == NULL) {

        p_task->handler = handler;
        p_task->data = data;
        p_task->state = TASK_STATE_RUNNING;
        p_task->status = 0;

        if (is_service) {
          SET_MASK(p_task->status, IS_SERVICE_MASK);
        }

        if (id >= tasks_info.max_id) {
          tasks_info.max_id++;
        }

        return id;
      }
    }
  }
  return INVALID_ID;
}

void deregister_task(task_id_t task_id) {
  if (task_id < tasks_info.max_id) {
    task_t *p_task = &TASK(task_id);

    if (p_task->handler) {
      p_task->handler = NULL;

      if (p_task->p_cr_state) *(p_task->p_cr_state) = 0;

      p_task->p_cr_state = 0;
      p_task->state = TASK_STATE_SUSPENDED;
      p_task->status = 0;

      /* clear events */
      for (uint8_t id = 0; id < c_SYS_MAX_EVENTS; id++) {
        if (events_info.events[id].task_id == task_id) {
          deregister_event(id);
        }
      }

      /* discard timers */
      for (uint8_t id = 0; id < c_SYS_MAX_SW_TIMERS; id++) {
        if (SOFTWARE_TIMER(id).task_id == task_id) {
          discard_timer(id);
        }
      }

      /* release resources */
      for (uint8_t i = 0; i < SYS_RES_MAX; ++i) {
        release_resource_internal(BITMASK(task_id), i);
      }
    }
  }
}

void alive() {
#ifndef PORT_WDT_VECT
  wdt_reset();
  reset_task_timer();
#else
  wdt_reset();
#endif
}

/* wake up a sleeping task */
static
void wake_task_cb(timer_id_t t_id, task_data_t data) {
  task_t *p_task = data.ptr;

  if (p_task) {
    run_task(p_task);
    discard_timer(t_id);
  }
}

void sleep(uint16_t ms) {
  task_t *p_task = &CUR_TASK;

  if (p_task->state != TASK_STATE_SLEEPING) {
    timer_id_t timer_id = get_timer();

    if (timer_id != INVALID_ID) {

      change_task_state(p_task, TASK_SIG_SLEEP);

      SET_TIMER(
          timer_id,
          ms,
          FALSE,
          wake_task_cb,
          (task_data_t)(void*)p_task
          );
      start_timer(timer_id);
    }
  }
}

static
void run_task_cb(event_id_t ev_id, task_data_t data) {
  task_t *p_task = data.ptr;

  if (p_task) {
    if (run_task(p_task) == TASK_STATE_RUNNING) {
      deregister_event_handler(ev_id, run_task_cb);
    }
  }
}

void wait_on_event(event_id_t ev_id) {
  task_t *p_task = &CUR_TASK;

  if (p_task->state == TASK_STATE_WAITING) {
    return;
  }

  if (register_event_handler(ev_id, run_task_cb, (task_data_t)(void*)p_task)) {
    return;
  }

  change_task_state(p_task, TASK_SIG_WAIT);
}

void suspend() {
  suspend_task(&CUR_TASK);
}

boolean resumed() {
  return (CUR_TASK.status & WAS_RESUMED_MASK);
}

boolean restarted() {
  return (CUR_TASK.status & WAS_RESTARTED_MASK);
}

/*****************************************************************************/
/* Resource control                                                          */
/*****************************************************************************/

#ifdef __ENABLE_RESMGMT
uint8_t acquire_resource(resource_id_t res, uint8_t shared, uint8_t forced) {
  uint8_t ret = ERR_RESOURCE_DENIED;
  resource_info_t *p_res = &resources[res];

  /* current task already owns the resource */
  if (p_res->status & CUR_TASK_MASK) {
    return 0;
  }

  /* a task has forced sole ownership */
  if (IS_RESOURCE_FORCED(p_res->status)) {
    return ret;
  }

  /* maximum owner count has been reached */
  if ((p_res->status & RESOURCE_OWNERS_MASK) >= RESOURCE_OWNERS_MASK) {
    return ret;
  }

  if (forced) {
    /* task is forcing sole ownership */
    p_res->prev_status = p_res->status;
    p_res->status = RESOURCE_FORCED_MASK;
    ret = 0;
  } else {
    /* no task owns this resource */
    if (!(p_res->status & RESOURCE_OWNERS_MASK)) {
      if (shared) {
        SET_MASK(p_res->status, RESOURCE_SHARED_MASK);
      }
      ret = 0;
    } else if (IS_RESOURCE_SHARED(p_res->status)) {
      /* resource is owned but available for sharing */
      ret = 0;
    }
  }

  if (!ret) {
    SET_MASK(p_res->status, CUR_TASK_MASK);
  }

  return ret;
}
#else
uint8_t acquire_resource(UNUSED_VARIABLE resource_id_t res,
    UNUSED_VARIABLE uint8_t shared, UNUSED_VARIABLE uint8_t forced) {
  return 0;
}
#endif

#ifdef __ENABLE_RESMGMT
uint8_t acquire_resource_mask(uint8_t mask, uint8_t shared_mask,
    uint8_t forced_mask) {

  uint8_t assigned_mask = 0;

  for (uint8_t i = 0; i < SYS_RES_MAX; ++i) {
    if (mask & 1) {
      if (acquire_resource(i, shared_mask & 1, forced_mask & 1)) {
        assigned_mask |= mask & 1;
      }
    }

    mask >>= 1;
    shared_mask >>= 1;
    forced_mask >>= 1;
  }

  return assigned_mask;
}
#else
uint8_t acquire_resource_mask(uint8_t mask,
    UNUSED_VARIABLE uint8_t shared_mask, UNUSED_VARIABLE uint8_t forced_mask) {
  return mask;
}
#endif

#ifdef __ENABLE_RESMGMT
void release_resource(resource_id_t res) {
  release_resource_internal(CUR_TASK_MASK, res);
}

void release_all_resources() {
  for (uint8_t i = 0; i < SYS_RES_MAX; ++i) {
    release_resource_internal(CUR_TASK_MASK, i);
  }
}
#else
void release_resource(UNUSED_VARIABLE resource_id_t res) {
}

void release_all_resources() {
}
#endif

#ifdef __ENABLE_RESMGMT
inline
__attribute__((always_inline))
uint8_t check_resource_owner(resource_id_t res) {
  /*
  return (CUR_TASK_ID < INVALID_ID) ? \
    resources[res].status & CUR_TASK_MASK : 1;
  */
  //return !!( resources[res].status & CUR_TASK_MASK );
#ifdef GPIOR3
  GPIOR3 = res;
#else
  asm volatile (
      "mov r24, %0"  "\n\t"
      :
      : "r" (res)
      : "r24"
  );
#endif

  trap(TRAP_TYPE_RES_CHK);

#ifdef GPIOR3
  return GPIOR3;
#else
  uint8_t ret;
  asm volatile (
      "mov %0, r24"   "\n\t"
      : "=r" (ret)
      :
      : "r24"
  );
  return ret;
#endif
}
#else
uint8_t check_resource_owner(UNUSED_VARIABLE resource_id_t res) {
  return 1;
}
#endif
