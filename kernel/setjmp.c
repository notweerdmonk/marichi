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
 * @file setjmp.c
 * @author notweerdmonk
 * @brief implements setjmp and longjmp
 */

#include <setjmp.h>

/*****************************************************************************/
/* Non-local goto                                                             */
/*****************************************************************************/

/**
 * Transfer control from one function to a predetermined location in another
 * function.
 */

/**
 * setjmp is used to mark the point to which control will be transferred. This
 * contains the information about the execution context (the environment of the
 * calling function).
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
      "std Z+15, r15"            "\n\t"
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
      "in __tmp_reg__, __SREG__" "\n\t"
      "std Z+32, __tmp_reg__"    "\n\t"
      "in __tmp_reg__, __SP_H__" "\n\t"
      "std Z+33, __tmp_reg__"    "\n\t"
      "in __tmp_reg__, __SP_L__" "\n\t"
      "std Z+34, __tmp_reg__"    "\n\t"
      "rcall getpc%="            "\n\t"
      "rjmp end%="               "\n\t"
      "getpc%=:"                 "\n\t"
      "pop r16"                  "\n\t"
      "pop r17"                  "\n\t"
      "std Z+35, r16"            "\n\t" /* MSB of PC */
      "std Z+36, r17"            "\n\t" /* LSB of PC */
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
