/***************************************************************************
 *
 * atomic-i86.s
 *
 * $Id: //stdlib/dev/source/stdlib/atomic-i86.s#16 $
 *
 ***************************************************************************
 *
 * Copyright (c) 1994-2005 Quovadx,  Inc., acting through its  Rogue Wave
 * Software division. Licensed under the Apache License, Version 2.0 (the
 * "License");  you may  not use this file except  in compliance with the
 * License.    You    may   obtain   a   copy   of    the   License    at
 * http://www.apache.org/licenses/LICENSE-2.0.    Unless   required    by
 * applicable law  or agreed to  in writing,  software  distributed under
 * the License is distributed on an "AS IS" BASIS,  WITHOUT WARRANTIES OR
 * CONDITIONS OF  ANY KIND, either  express or implied.  See  the License
 * for the specific language governing permissions  and limitations under
 * the License.
 * 
 **************************************************************************/

    .text
    .align 4

/***************************************************************************
 * extern "C" char __rw_atomic_xchg8 (char *x, char y);
 *
 * Atomically assigns the 8-bit value y to *x and returns
 * the original (before assignment) 8-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_xchg8
    .type __rw_atomic_xchg8, @function
__rw_atomic_xchg8:                 /* ; char (char *x, char y)        */
    mov           4(%esp), %ecx    /* ; %ecx = x                      */
    movb          8(%esp), %al     /* ; %al = y                       */
    xchg          %al, (%ecx)      /* ; %al <-> (%ecx)                */
    ret


/***************************************************************************
 * extern "C" short __rw_atomic_xchg16 (short *x, short y);
 *
 * Atomically assigns the 16-bit value y to *x and returns
 * the original (before assignment) 16-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_xchg16
    .type __rw_atomic_xchg16, @function
__rw_atomic_xchg16:                /* ; short (short *x, short y)     */
    mov           4(%esp), %ecx    /* ; %ecx = x                      */
    mov           8(%esp), %eax    /* ; %eax = y                      */
    xchgw         %ax, (%ecx)      /* ; %ax <-> (%ecx)                */
    ret


/***************************************************************************
 * extern "C" int __rw_atomic_xchg (int *x, int y);
 *
 * Atomically assigns the 32-bit value y to *x and returns
 * the original (before assignment) 32-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_xchg
    .type __rw_atomic_xchg, @function
__rw_atomic_xchg:                  /* ; int (int *x, int y)           */
    movl          4(%esp), %ecx    /* ; %ecx = x                      */
    movl          8(%esp), %eax    /* ; %eax = y                      */
    xchgl         %eax, (%ecx)     /* ; %eax <-> (%ecx)               */
    ret


/***************************************************************************
 * extern "C" char __rw_atomic_add8 (char *x, int y);
 *
 * Atomically increments the 8-bit value *x by y and returns
 * the new (after increment) 8-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_add8
    .type __rw_atomic_add8, @function
__rw_atomic_add8:                  /* ; char (char *dst, int inc)     */
    mov            4(%esp), %ecx   /* ; %ecx = dst                    */
    mov            8(%esp), %eax   /* ; %eax = inc                    */
    mov            %eax, %edx      /* ; */

    lock
    xaddb          %al, (%ecx)     /* ; tmp = *dst;
                                      ; dst += inc;
                                      ; %al = tmp                     */

    add            %edx, %eax      /* ; return %eax + inc             */
    ret


/***************************************************************************
 * extern "C" short __rw_atomic_add16 (short *x, short y);
 *
 * Atomically increments the 16-bit value *x by y and returns
 * the new (after increment) 16-bit value of *x.
 **************************************************************************/

     .globl __rw_atomic_add16
    .type __rw_atomic_add16, @function
__rw_atomic_add16:                 /* ; long (long *dst, long inc)    */
    mov            4(%esp), %ecx   /* ; %ecx = dst                    */
    mov            8(%esp), %eax   /* ; %eax = inc                    */
    mov            %eax, %edx      /* ; */

    lock
    xaddw          %ax, (%ecx)     /* ; tmp = *dst;
                                      ; dst += inc;
                                      ; eax = tmp                     */

    add            %edx, %eax      /* ; return %eax + inc             */
    ret


/***************************************************************************
 * extern "C" int __rw_atomic_add (int *x, int y);
 *
 * Atomically increments the 32-bit value *x by y and returns
 * the new (after increment) 32-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_add
    .type __rw_atomic_add, @function
__rw_atomic_add:                   /* ; long (long *dst, long inc)    */
    movl           4(%esp), %ecx   /* ; %ecx = dst                    */
    movl           8(%esp), %edx   /* ; %edx = inc                    */
    movl           %edx, %eax      /* ; */

    lock
    xaddl          %eax, (%ecx)    /* ; tmp = *dst;
                                      ; dst += inc;
                                      ; %eax = tmp                    */

    addl           %edx, %eax      /* ; return %eax + inc             */
    ret
