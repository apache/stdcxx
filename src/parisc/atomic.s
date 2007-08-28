/*****************************************************************************
 *
 * parisc/atomic.s
 *
 * $Id$
 *
 *****************************************************************************
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
 * Copyright 2002-2006 Rogue Wave Software.
 *
 * 2002 Copyright Hewlett-Packard Company
 *
 * Routines for atomic increment and decrement of int32_t values
 *
 * int32_t __rw_atomic_incr32(int32_t *addr);
 * int32_t __rw_atomic_decr32(int32_t *addr);
 * int32_t __rw_atomic_xchg32(int32_t *addr, int32_t val);
 *
 *****************************************************************************/

#if !defined (_PA_RISC2_0)
#  define _PA_RISC2_0
#endif // _PA_RISC2_0

#define addr       arg0
#define swap_val   arg1
#define tmp1       r22
#define tmp2       r20
#define interlock_val r31

/*
 * For pre HP-UX 11.0 compilers, this will need to change to C comment
 * Style given the compilers are broken and don't use ## correctly
*/
#define LABEL(a,b)     .label a##b
#define LABEL_REF(a,b)  a##b

#ifdef __LP64__
        .level 2.0W
#define LDD ldd
#define STD std
#define STACK_ADJUST -16
#define FRAME_SIZE 48
#define RP_OFFSET -144
#define LOCK_OFFSET -120
#define ADDR_OFFSET -112
#define SWAP_OFFSET -104
#define DP r27
#define SAVE_DATA_POINTER       std  DP,-128(sp)
#define RESTORE_DATA_POINTER    ldd  -128(sp),DP
#define RETURN_SEQ(tmpreg) bve  (rp) ! ldo -128(sp),sp
#define SAVE_ARG_POINTER   ldo -48(sp),r29
#else
#ifdef _PA_RISC2_0
        .level 2.0
#define RETURN_SEQ(tmpreg) bve  (rp) ! ldo -128(sp),sp
#else
        .level 1.1
    error, can't use this code on PA-RISC 1.1 implementations
#endif
#define LDD ldw
#define STD stw
#define STACK_ADJUST -20
#define FRAME_SIZE 16
#define RP_OFFSET -148
#define SWAP_OFFSET -44
#define LOCK_OFFSET -40
#define ADDR_OFFSET -36
#define DP r19
#define SAVE_DATA_POINTER       stw  DP,-32(sp)
#define RESTORE_DATA_POINTER    ldw  -32(sp),DP
#define SAVE_ARG_POINTER
#endif

#define __rwInterlocked_BODY(sn, BODY)                                             \
        STD      rp,STACK_ADJUST(sp)                /* store return pointer */    !\
        ldo      128(sp),sp                         /* adjust stack */            !\
LABEL(sn,_top)                                                                    !\
        ldcws,co 0(addr),interlock_val              /* grab lock */               !\
                                                    /* if lock held, then spin */ !\
        cmpb,=,n  r0,interlock_val, LABEL_REF(sn,_spin_top)                       !\
                                                    /* branch pred not taken */   !\
        LDD     RP_OFFSET(sp),rp                    /* load return pointer  */    !\
                                                    /* Free variables are      */ !\
                                                    /* tmp1, tmp2   */            !\
    BODY                                            /* Do operation */            !\
        RETURN_SEQ(tmp1)                            /* return sequence */         !\
                                                                                  !\
LABEL(sn,_spin_top)                                                               !\
/*                                                                                !\
 * Did not get the lock, spin for it for a little while                           !\
 */                                                                               !\
        depwi,z 1,15,1,tmp2             /* spin 65536 times, then go to higher */ !\
                                        /* level code */                          !\
        ldws     0(addr), tmp1          /* look at lock value */                  !\
LABEL(sn,_loop)                                                                   !\
        cmpb,<>  tmp1, r0, LABEL_REF(sn,_top) /* got the lock, then try again */  !\
                                        /* atomically, predict not taken */       !\
        nop                                                                       !\
        addib,<>,n -1, tmp2, LABEL_REF(sn,_loop) /* see if lock is free again */  !\
        ldws     0(addr), tmp1          /* look at lock value */                  !\
                                                                                  !\
        cmpb,<>  r0,tmp2,LABEL_REF(sn,_top) /* if exited due to getting lock*/    !\
                                            /* then try again atomically */       !\
                                            /* predicted not taken */             !\
        nop                                                                       !\
                                                                                  !\
        /*                                                                        !\
         * Did not get the lock spinning, so yield the processor to allow         !\
         * someone else a shot                                                    !\
         */                                                                       !\
        STD      addr,ADDR_OFFSET(sp)       /* save the address to access */      !\
        STD      swap_val,SWAP_OFFSET(sp)   /* save the swap value    */          !\
        SAVE_ARG_POINTER                                                          !\
        .call rtnval=gr                                                           !\
        b,l sched_yield,rp               /* yield the processor and try again */  !\
        SAVE_DATA_POINTER                                                         !\
        LDD     SWAP_OFFSET(sp),swap_val /* restore the address of the lock */    !\
        LDD     ADDR_OFFSET(sp),addr     /* restore the address to access */      !\
        RESTORE_DATA_POINTER                                                      !\
        b   LABEL_REF(sn,_spin_top)      /* Try again */                          !\
        nop

;
; Export all of the interfaces
;
    .space  $TEXT$
    .subspa $CODE$
        .export __rw_atomic_incr32,entry,argw0=gr,rtnval=gr,LONG_RETURN
        .export __rw_atomic_decr32,entry,argw0=gr,rtnval=gr,LONG_RETURN
        .export __rw_atomic_xchg32,entry,argw0=gr,rtnval=gr,LONG_RETURN

;-------------------------------------------------------------------------------
; int32_t __rw_atomic_incr32(int32_t *addr);
; arg0 = int32_t *addr
;
; This routine atomically increments the passed in value and returns
; the newly incremented value.  It is atomic with respect to other
; __rw_atomic_incr32 calls and with respect to other __rw_atomic_decr32
; calls.
;-------------------------------------------------------------------------------
#define IncrementBODY                                                       \
        ldo     1(interlock_val),ret0   /* adjust value  */                !\
        stw     ret0,0(addr)                /* store new result, release lock */

        .code
__rw_atomic_incr32
        .proc
        .import sched_yield
        .callinfo caller,frame=FRAME_SIZE,save_rp,args_saved
        __rwInterlocked_BODY(ii, IncrementBODY)
        .procend


;-------------------------------------------------------------------------------
; int32_t __rw_atomic_decr32(int32_t *addr);
; arg0 = int32_t *addr
;
; This routine atomically decrements the passed in value and returns
; the newly decremented value.  It is atomic with respect to other
; __rw_atomic_decr32 calls and with respect to other __rw_atomic_incr32
; calls.
;-------------------------------------------------------------------------------

#define DecrementBODY                                                     \
        ldo     -1(interlock_val),ret0      /* adjust value  */              !\
        stw     ret0,0(addr)                /* store new result,release lock */

__rw_atomic_decr32
        .proc
        .import sched_yield
        .callinfo caller,frame=FRAME_SIZE,save_rp,args_saved
        __rwInterlocked_BODY(id, DecrementBODY)
        .procend

#define ExchangeBODY                                                      \
        ldo   0(interlock_val),ret0    /* return current value */        !\
        stw   swap_val, 0(addr)        /* store new value, release lock */

__rw_atomic_xchg32
        .proc
        .import sched_yield
        .callinfo caller,frame=FRAME_SIZE,save_rp,args_saved

        __rwInterlocked_BODY(is, ExchangeBODY)
        .procend
