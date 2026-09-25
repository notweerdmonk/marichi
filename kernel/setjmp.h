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
 * @file setjmp.h
 * @author notweerdmonk
 * @brief implements setjmp
 */

#ifndef _AVR_KERNEL_SETJMP_H_
#define _AVR_KERNEL_SETJMP_H_

#include <common.h>

typedef struct _jump_buf {
  uint8_t buf[38];
} jump_buf_t[1];

/*****************************************************************************/
/* Nonlocal goto                                                             */
/*****************************************************************************/

/**
 * Macro for longjmp
 *
 * A ret instruction needs to follow the macro for longjmp to work. This
 * is done by wrapping the macro in a function. The ret instruction will put
 * the address at the top of stack in program counter. But the instruction is
 * ommited in the macro because calling the macro form an ISR will require an
 * reti instruction instead, and can be added as required.
 */
#define LONGJMP(buf) \
  asm volatile ( \
    "ldd r1, Z+1"               "\n\t"                 \
    "ldd r2, Z+2"               "\n\t"                 \
    "ldd r3, Z+3"               "\n\t"                 \
    "ldd r4, Z+4"               "\n\t"                 \
    "ldd r5, Z+5"               "\n\t"                 \
    "ldd r6, Z+6"               "\n\t"                 \
    "ldd r7, Z+7"               "\n\t"                 \
    "ldd r8, Z+8"               "\n\t"                 \
    "ldd r9, Z+9"               "\n\t"                 \
    "ldd r10, Z+10"             "\n\t"                 \
    "ldd r11, Z+11"             "\n\t"                 \
    "ldd r12, Z+12"             "\n\t"                 \
    "ldd r13, Z+13"             "\n\t"                 \
    "ldd r14, Z+14"             "\n\t"                 \
    "ldd r15, Z+15"             "\n\t"                 \
    "ldd r16, Z+16"             "\n\t"                 \
    "ldd r17, Z+17"             "\n\t"                 \
    "ldd r18, Z+18"             "\n\t"                 \
    "ldd r19, Z+19"             "\n\t"                 \
    "ldd r20, Z+20"             "\n\t"                 \
    "ldd r21, Z+21"             "\n\t"                 \
    "ldd r22, Z+22"             "\n\t"                 \
    "ldd r23, Z+23"             "\n\t"                 \
    "ldd r24, Z+24"             "\n\t"                 \
    "ldd r25, Z+25"             "\n\t"                 \
    "ldd r26, Z+26"             "\n\t"                 \
    "ldd r27, Z+27"             "\n\t"                 \
    "ldd r28, Z+28"             "\n\t"                 \
    "ldd r29, Z+29"             "\n\t"                 \
    "ldd __tmp_reg__, Z+32"     "\n\t" /* SP_H */      \
    "out __SP_H__, __tmp_reg__" "\n\t"                 \
    "ldd __tmp_reg__, Z+33"     "\n\t" /* SP_L */      \
    "out __SP_L__, __tmp_reg__" "\n\t"                 \
    "ldd __tmp_reg__, Z+35"     "\n\t" /* LSB of PC */ \
    "push __tmp_reg__"          "\n\t"                 \
    "ldd __tmp_reg__, Z+34"     "\n\t" /* MSB of PC */ \
    "push __tmp_reg__"          "\n\t"                 \
    "ldd __tmp_reg__, Z+30"     "\n\t" /* r30 */       \
    "push __tmp_reg__"          "\n\t"                 \
    "ldd __tmp_reg__, Z+31"     "\n\t" /* r31 */       \
    "push __tmp_reg__"          "\n\t"                 \
    "pop r31"                   "\n\t"                 \
    "pop r30"                   "\n\t"                 \
    "ld r0, Z"                  "\n\t"                 \
    :                                                  \
    : "z" (buf)                                        \
    : "r0"                                             \
  );

/**
 * @brief Exposes kernel internal task reset jump buffer as const pointer.
 * @return const jump_buf_t* Pointer to the internal task reset jump buffer
 */
const jump_buf_t* kernel_task_reset_jump_buffer();

void setjmp(jump_buf_t buf);

void longjmp(jump_buf_t buf);

#endif /* _AVR_KERNEL_SETJMP_H_ */
