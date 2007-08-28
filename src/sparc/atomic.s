!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!
! sparc/atomic.s
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
! extern "C" int __rw_atomic_add32 (int *x, int y);
!
! Atomically increments the 32-bit value value *x by y and returns
! the new (after increment) 32-bit value of *x.
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    .type __rw_atomic_add32, #function
    .global __rw_atomic_add32

__rw_atomic_add32:            ! (int *x: %o0, int y: %o1)
    mov     -1, %o2           ! INT_MAX is reserved
    srl     %o2, 1, %o2       ! %o2 = -1U >> 1 (== INT_MAX)
.__rw_retry:
    mov     %o2, %o3          ! make a copy for swap 
    swap    [%o0], %o3        ! swap *x for %o3
    cmp     %o3, %o2          ! if *x == INT_MAX (i.e., resource is locked)
    beq,a   .__rw_busywait    !     then busy-wait (else annul next insn)
    nop                       ! else perform add and return
    add     %o3, %o1, %o2     ! %o2 = *x + y
    st      %o2, [%o0]        ! store the sum in *x
    retl                      ! return to caller
    mov     %o2, %o0          ! (delayed) store result in %o0
.__rw_busywait:               ! busy-wait with loads (cheaper than swaps)
    cmp     %o3, %o2          ! if %o3 == INT_MAX
    be      .__rw_busywait    !     then spin
    ld      [%o0], %o3        ! (delayed) %o3 = *x
    ba,a    .__rw_retry       ! otherwise, try again for lock
    nop
