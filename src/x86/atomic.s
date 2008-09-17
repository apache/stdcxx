/***************************************************************************
 *
 * x86/atomic.s
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 2003-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#if defined (__ELF__)
   /* GNU as supports this .type directive only on ELF targets */
#  define TYPE_DIR(name, kind)    .type name, kind
#else
#  define TYPE_DIR(ign1, ign2)
#endif

#if defined (__CYGWIN__) || defined (__MINGW32__)
   /* on cygwin used COFF object file format
      C-functions names should prepended by '_' */
#  define SYMNAME(name)    _##name
#else
#  define SYMNAME(name)    name
#endif

    .text
    .align 4

/***************************************************************************
 * extern "C" char __rw_atomic_xchg8 (char *x, char y);
 *
 * Atomically assigns the 8-bit value y to *x and returns
 * the original (before assignment) 8-bit value of *x.
 **************************************************************************/

    .globl SYMNAME (__rw_atomic_xchg8)
    TYPE_DIR (__rw_atomic_xchg8, @function)

SYMNAME (__rw_atomic_xchg8):       /* ; char (char *x, char y)        */
    mov           4(%esp), %ecx    /* ; %ecx = x                      */
    movb          8(%esp), %al     /* ; %al = y                       */
    xchgb         %al, (%ecx)      /* ; %al <-> (%ecx)                */
    ret


/***************************************************************************
 * extern "C" short __rw_atomic_xchg16 (short *x, short y);
 *
 * Atomically assigns the 16-bit value y to *x and returns
 * the original (before assignment) 16-bit value of *x.
 **************************************************************************/

    .globl SYMNAME (__rw_atomic_xchg16)
    TYPE_DIR (__rw_atomic_xchg16, @function)

SYMNAME (__rw_atomic_xchg16):      /* ; short (short *x, short y)     */
    mov           4(%esp), %ecx    /* ; %ecx = x                      */
    mov           8(%esp), %eax    /* ; %eax = y                      */
    xchgw         %ax, (%ecx)      /* ; %ax <-> (%ecx)                */
    ret


/***************************************************************************
 * extern "C" int __rw_atomic_xchg32 (int *x, int y);
 *
 * Atomically assigns the 32-bit value y to *x and returns
 * the original (before assignment) 32-bit value of *x.
 **************************************************************************/

    .globl SYMNAME (__rw_atomic_xchg32)
    TYPE_DIR (__rw_atomic_xchg32, @function)

SYMNAME (__rw_atomic_xchg32):      /* ; int (int *x, int y)           */
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

    .globl SYMNAME (__rw_atomic_add8)
    TYPE_DIR (__rw_atomic_add8, @function)

SYMNAME (__rw_atomic_add8):        /* ; char (char *dst, int inc)     */
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

     .globl SYMNAME (__rw_atomic_add16)
    TYPE_DIR (__rw_atomic_add16, @function)

SYMNAME (__rw_atomic_add16):       /* ; short (short *dst, short inc) */
    mov            4(%esp), %ecx   /* ; %ecx = dst                    */
    mov            8(%esp), %eax   /* ; %eax = inc                    */
    mov            %eax, %edx      /* ; */

    lock
    xaddw          %ax, (%ecx)     /* ; tmp = *dst;
                                      ; dst += inc;
                                      ; %ax = tmp                     */

    add            %edx, %eax      /* ; return %eax + inc             */
    ret


/***************************************************************************
 * extern "C" int __rw_atomic_add32 (int *x, int y);
 *
 * Atomically increments the 32-bit value *x by y and returns
 * the new (after increment) 32-bit value of *x.
 **************************************************************************/

    .globl SYMNAME (__rw_atomic_add32)
    TYPE_DIR (__rw_atomic_add32, @function)

SYMNAME (__rw_atomic_add32):       /* ; int (int *dst, int inc)       */
    movl           4(%esp), %ecx   /* ; %ecx = dst                    */
    movl           8(%esp), %edx   /* ; %edx = inc                    */
    movl           %edx, %eax      /* ; */

    lock
    xaddl          %eax, (%ecx)    /* ; tmp = *dst;
                                      ; dst += inc;
                                      ; %eax = tmp                    */

    addl           %edx, %eax      /* ; return %eax + inc             */
    ret
