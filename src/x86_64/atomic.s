/***************************************************************************
 *
 * x86_64/atomic.s
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
 * Copyright 2005-2006 Rogue Wave Software.
 * 
 **************************************************************************/

#if defined (__MACH__)
// Mac OS X Mach-O assembler: no .type, power of two alignment
#  define ALIGN_DIR           .align 4
#  define TYPE_DIR(ignore,ignore2) 
#else
#  define ALIGN_DIR           .align 16
#  define TYPE_DIR(sym,attr)  .type sym, attr
#endif // __MACH__

    .text
    ALIGN_DIR

/***************************************************************************
 * extern "C" int8_t __rw_atomic_xchg8 (int8_t *x, int8_t y);
 *
 * Atomically assigns the 8-bit value y to *x and returns
 * the original (before assignment) 8-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_xchg8
    TYPE_DIR (__rw_atomic_xchg8, STT_FUNC)
__rw_atomic_xchg8:                 /* ; int8_t (int8_t *x, int8_t y)  */
    movq          %rdi, %rcx       /* ; %rcx = x                      */
    movb          %sil,  %al       /* ; %al = y                       */
    xchgb         %al, (%rcx)      /* ; %al <-> (%rcx)                */
    ret


/***************************************************************************
 * extern "C" int16_t __rw_atomic_xchg16 (int16_t *x, int16_t y);
 *
 * Atomically assigns the 16-bit value y to *x and returns
 * the original (before assignment) 16-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_xchg16
    TYPE_DIR (__rw_atomic_xchg16, STT_FUNC)
__rw_atomic_xchg16:                /* ; int16_t (int16_t *x, int16_t y) */
    movq          %rdi, %rcx       /* ; %rcx = x                        */
    movw          %si,  %ax        /* ; %ax = y                         */
    xchgw         %ax, (%rcx)      /* ; %ax <-> (%rcx)                  */
    ret


/***************************************************************************
 * extern "C" int32_t __rw_atomic_xchg32 (int32_t *x, int32_t y);
 *
 * Atomically assigns the 32-bit value y to *x and returns
 * the original (before assignment) 32-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_xchg32
    TYPE_DIR (__rw_atomic_xchg32, STT_FUNC)
__rw_atomic_xchg32:                /* ; int32_t (int32_t *x, int32_t y) */
    movq          %rdi,  %rcx      /* ; %rcx = x                        */
    movl          %esi,  %eax      /* ; %eax = y                        */
    xchgl         %eax, (%rcx)     /* ; %eax <-> (%rcx)                 */
    ret


/***************************************************************************
 * extern "C" int32_t __rw_atomic_xchg64 (int64_t *x, int64_t y);
 *
 * Atomically assigns the 64-bit value y to *x and returns
 * the original (before assignment) 64-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_xchg64
    TYPE_DIR (__rw_atomic_xchg64, STT_FUNC)
__rw_atomic_xchg64:                /* ; int64_t (int64_t *x, int64_t y) */
    movq          %rdi,  %rcx      /* ; %rcx = x                        */
    movq          %rsi,  %rax      /* ; %rax = y                        */
    xchgq         %rax, (%rcx)     /* ; %rax <-> (%rcx)                 */
    ret


/***************************************************************************
 * extern "C" int8_t __rw_atomic_add8 (int8_t *x, int8_t y);
 *
 * Atomically increments the 8-bit value *x by y and returns
 * the new (after increment) 8-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_add8
    TYPE_DIR (__rw_atomic_add8, STT_FUNC)
__rw_atomic_add8:                  /* ; int8_t (int8_t *dst, int8_t inc) */
    movq           %rdi, %rcx      /* ; %rcx = dst                       */
    movl           %esi, %eax      /* ; %eax = inc                       */
    movl           %eax, %edx      /* ;                                  */

    lock                           /* ; lock the bus                     */
    xaddb          %al, (%rcx)     /* ; tmp = *dst                       */
                                   /* ; dst += inc                       */
                                   /* ; %al = tmp                        */
    addl           %edx, %eax      /* ; return %al + inc                 */
    ret


/***************************************************************************
 * extern "C" int16_t __rw_atomic_add16 (int16_t *x, int16_t y);
 *
 * Atomically increments the 16-bit value *x by y and returns
 * the new (after increment) 16-bit value of *x.
 **************************************************************************/

     .globl __rw_atomic_add16
    TYPE_DIR (__rw_atomic_add16, STT_FUNC)
__rw_atomic_add16:                 /* ; int16_t (int16_t *dst, int16_t inc) */
    movq           %rdi, %rcx      /* ; %rcx = dst                          */
    movw           %si,  %ax       /* ; %ax = inc                           */
    movw           %ax,  %dx       /* ;                                     */

    lock                           /* ; lock the bus                        */
    xaddw          %ax, (%rcx)     /* ; tmp = *dst                          */
                                   /* ; dst += inc                          */
                                   /* ; %ax = tmp                            */

    addw           %dx,  %ax       /* ; return %ax + inc                    */
    ret


/***************************************************************************
 * extern "C" int32_t __rw_atomic_add32 (int32_t *x, int32_t y);
 *
 * Atomically increments the 32-bit value *x by y and returns
 * the new (after increment) 32-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_add32
    TYPE_DIR (__rw_atomic_add32, STT_FUNC)
__rw_atomic_add32:                 /* ; int32_t (int32_t *dst, int32_t inc) */
    movq           %rdi, %rcx      /* ; %rcx = dst                          */
    movl           %esi, %edx      /* ; %edx = inc                          */
    movl           %edx, %eax      /* ;                                     */

    lock                           /* ; lock the bus                        */
    xaddl          %eax, (%rcx)    /* ; tmp = *dst                          */
                                   /* ; dst += inc                          */
                                   /* ; %eax = tmp                          */

    addl           %edx, %eax      /* ; return %eax + inc                   */
    ret


/***************************************************************************
 * extern "C" int64_t __rw_atomic_add64 (int64_t *x, int64_t y);
 *
 * Atomically increments the 32-bit value *x by y and returns
 * the new (after increment) 32-bit value of *x.
 **************************************************************************/

    .globl __rw_atomic_add64
    TYPE_DIR (__rw_atomic_add64, STT_FUNC)
__rw_atomic_add64:                 /* ; int64_t (int64_t *dst, int64_t inc) */
    movq           %rdi, %rcx      /* ; %rcx = dst                          */
    movq           %rsi, %rdx      /* ; %edx = inc                          */
    movq           %rdx, %rax      /* ;                                     */

    lock                           /* ; lock the bus                        */
    xaddq          %rax, (%rcx)    /* ; tmp = *dst                          */
                                   /* ; dst += inc                          */
                                   /* ; %rax = tmp                          */

    addq           %rdx, %rax      /* ; return %rax + inc                   */
    ret
