!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!
! sparc/atomic-64.s
!
! $Id$
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!
! Licensed to the Apache Software  Foundation (ASF) under one or more
! contributor  license agreements.  See  the NOTICE  file distributed
! with  this  work  for  additional information  regarding  copyright
! ownership.   The ASF  licenses this  file to  you under  the Apache
! License, Version  2.0 (the  "License"); you may  not use  this file
! except in  compliance with the License.   You may obtain  a copy of
! the License at
!
! http://www.apache.org/licenses/LICENSE-2.0
!
! Unless required by applicable law or agreed to in writing, software
! distributed under the  License is distributed on an  "AS IS" BASIS,
! WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
! implied.   See  the License  for  the  specific language  governing
! permissions and limitations under the License.
!
! Copyright 2002-2006 Rogue Wave Software.
!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    .section ".text"
    .align 8


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! extern "C" int __rw_atomic_xchg32 (int *x, int y);
!
! Atomically assigns the 32-bit value value y to *x and returns
! the original (before assignment) 32-bit value of *x.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    .type __rw_atomic_xchg32, #function
    .global __rw_atomic_xchg32

__rw_atomic_xchg32:           ! (int *x: %o0, int y: %o1)
    swap    [%o0], %o1        ! *x <-> y
    retl
    mov     %o1, %o0          ! return original value of *x


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! extern "C" int __rw_atomic_xchg64 (long *x, long y);
!
! Atomically assigns the 64-bit value value y to *x and returns
! the original (before assignment) 64-bit value of *x.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    .type __rw_atomic_xchg64, #function
    .global __rw_atomic_xchg64

__rw_atomic_xchg64:            ! (long *x: %o0, long y: %o1)
    mov     %o1, %o3           ! %o3 <- y
.__rw_retry_xchg64:
    ldx     [%o0], %o2         ! %o2 <- *x
    casx    [%o0], %o2, %o3    ! atomic operation:
                               !   if [%o0] == %o2, [%o0] <-> %o3
                               !   else %o3 <- [%o0]
    cmp     %o2, %o3           ! if swap succeeded, both %o2 and %o3
                               !   contain the original value of *x
    bne,a   .__rw_retry_xchg64 ! otherwise goto retry
    mov     %o1, %o3           ! %o3 <- y
    retl
    mov     %o3, %o0           ! return result (old value)


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! extern "C" int __rw_atomic_add32 (int *x, int y);
!
! Atomically increments the 32-bit value value *x by y and returns
! the new (after increment) 32-bit value of *x.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    .type __rw_atomic_add32, #function
    .global __rw_atomic_add32

__rw_atomic_add32:            ! (int *x: %o0, int y: %o1)
.__rw_retry_add32:
    ld      [%o0], %o2        ! load *x into %o2
    add     %o2, %o1, %o3     ! %o3 = %o1 + %o2
    cas     [%o0], %o2, %o3   ! atomic operation:
                              !   if [%o0] == %o2, swap ([%o0], %o3)
                              !   else copy [%o0] into %o3
    cmp     %o2, %o3          ! if swap succeeded %o2 and %o3 are
                              !   both the old value
    bne     .__rw_retry_add32 ! otherwise retry
    nop
    add     %o3, %o1, %o0     ! return result (new value)
    retl                     
    nop


!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
! extern "C" int __rw_atomic_add64 (long *x, long y);
!
! Atomically increments the 64-bit value value *x by y and returns
! the new (after increment) 64-bit value of *x.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    .type __rw_atomic_add64, #function
    .global __rw_atomic_add64

__rw_atomic_add64:            ! (long *x: %o0, long y: %o1)
.__rw_retry_add64:
    ldx     [%o0], %o2        ! load *x into %o2
    add     %o2, %o1, %o3     ! %o3 = %o1 + %o2
    casx    [%o0], %o2, %o3   ! atomic operation:
                              !   if [%o0] == %o2, swap ([%o0], %o3)
                              !   else copy [%o0] into %o3
    cmp     %o2, %o3          ! if swap succeeded %o2 and %o3 are
                              !   both the old value
    bne     .__rw_retry_add64 ! otherwise retry
    nop
    add     %o3, %o1, %o0     ! return result (new value)
    retl                     
    nop
