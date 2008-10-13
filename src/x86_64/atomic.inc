;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; x86_64/atomic.inc
;
; $Id$
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; Licensed to the Apache Software  Foundation (ASF) under one or more
; contributor  license agreements.  See  the NOTICE  file distributed
; with  this  work  for  additional information  regarding  copyright
; ownership.   The ASF  licenses this  file to  you under  the Apache
; License, Version  2.0 (the  "License"); you may  not use  this file
; except in  compliance with the License.   You may obtain  a copy of
; the License at
;
; http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the  License is distributed on an  "AS IS" BASIS,
; WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
; implied.   See  the License  for  the  specific language  governing
; permissions and limitations under the License.
;
; Copyright 2003-2006 Rogue Wave Software.
; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


                .code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int8_t __rw_atomic_xchg8 (int8_t *x, int8_t y);
;
; Atomically assigns the 8-bit value y to *x and returns
; the original (before assignment) 8-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 16
                public __rw_atomic_xchg8
__rw_atomic_xchg8 proc                    ; int8_t (int8_t *x, int8_t y)
                                          ; %rcx = x
                mov     al, dl            ; %al = y 
                xchg    al, [rcx]         ; %al <-> (%rcx)
                ret
__rw_atomic_xchg8 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int16_t __rw_atomic_xchg16 (int16_t *x, int16_t y);
;
; Atomically assigns the 16-bit value y to *x and returns
; the original (before assignment) 16-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 16
                public __rw_atomic_xchg16
__rw_atomic_xchg16 proc                   ; int16_t (int16_t *x, int16_t y)
                                          ; %rcx = x
                mov     ax, dx            ; %ax = y
                xchg    ax, [rcx]         ; %ax <-> (%rcx)
                ret
__rw_atomic_xchg16 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int32_t __rw_atomic_xchg32 (int32_t *x, int32_t y);
;
; Atomically assigns the 32-bit value y to *x and returns
; the original (before assignment) 32-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 16
                public __rw_atomic_xchg32
__rw_atomic_xchg32 proc                   ; int32_t (int32_t *x, int32_t y)
                                          ; %rcx = x
                mov     eax, edx          ; %eax = y
                xchg    eax, [rcx]        ; %eax <-> (%rcx)
                ret
__rw_atomic_xchg32 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int32_t __rw_atomic_xchg64 (int64_t *x, int64_t y);
;
; Atomically assigns the 64-bit value y to *x and returns
; the original (before assignment) 64-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 16
                public __rw_atomic_xchg64
__rw_atomic_xchg64 proc                   ; int64_t (int64_t *x, int64_t y)
                                          ; %rcx = x
                mov     rax, rdx          ; %rax = y
                xchg    rax, [rcx]        ; %rax <-> (%rcx)
                ret
__rw_atomic_xchg64 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int8_t __rw_atomic_add8 (int8_t *x, int8_t y);
;
; Atomically increments the 8-bit value *x by y and returns
; the new (after increment) 8-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 16
                public __rw_atomic_add8
__rw_atomic_add8 proc                     ; int8_t (int8_t *dst, int8_t inc)
                                          ; %rcx = dst
                mov     eax, edx          ; %eax = inc

                lock xadd [rcx], al       ; tmp = *dst
                                          ; dst += inc      
                                          ; %al = tmp       
                add     eax, edx          ; return %al + inc
                ret
__rw_atomic_add8 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int16_t __rw_atomic_add16 (int16_t *x, int16_t y);
;
; Atomically increments the 16-bit value *x by y and returns
; the new (after increment) 16-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 16
                public __rw_atomic_add16
__rw_atomic_add16 proc                    ; int16_t (int16_t *dst, int16_t inc)
                                          ; %rcx = dst
                mov     ax,  dx           ; %ax = inc 

                lock xadd [rcx], ax       ; tmp = *dst
                                          ; dst += inc
                                          ; eax = tmp 

                add     ax,  dx           ; return %ax + inc
                ret
__rw_atomic_add16 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int32_t __rw_atomic_add32 (int32_t *x, int32_t y);
;
; Atomically increments the 32-bit value *x by y and returns
; the new (after increment) 32-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 16
                public __rw_atomic_add32
__rw_atomic_add32 proc                    ; int32_t (int32_t *dst, int32_t inc)
                                          ; %rcx = dst
                mov     eax, edx          ; %eax = inc

                lock xadd [rcx], eax      ; tmp = *dst
                                          ; dst += inc
                                          ; %eax = tmp

                add     eax, edx          ; return %eax + inc
                ret
__rw_atomic_add32 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int64_t __rw_atomic_add64 (int64_t *x, int64_t y);
;
; Atomically increments the 32-bit value *x by y and returns
; the new (after increment) 32-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 16
                public __rw_atomic_add64
__rw_atomic_add64 proc                    ; int64_t (int64_t *dst, int64_t inc)
                                          ; %rcx = dst
                mov     rax, rdx          ; %eax = inc

                lock xadd [rcx], rax      ; tmp = *dst
                                          ; dst += inc
                                          ; %eax = tmp

                add     rax, rdx          ; return %eax + inc
                ret
__rw_atomic_add64 endp
