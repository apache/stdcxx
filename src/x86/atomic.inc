;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; x86/atomic.inc
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


                .486
                .model flat
                .code

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" char __rw_atomic_xchg8 (char *x, char y);
;
; Atomically assigns the 8-bit value y to *x and returns
; the original (before assignment) 8-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 4
                public ___rw_atomic_xchg8
___rw_atomic_xchg8 proc                     ; char (char *x, char y)

arg_x           = dword ptr  4
arg_y           = byte ptr  8
                                            
                mov     ecx, [esp+arg_x]    ; %ecx = x
                mov     al,  [esp+arg_y]    ; %al = y
                xchg    al,  [ecx]          ; %al <-> (%ecx)
                ret                        
___rw_atomic_xchg8 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" short __rw_atomic_xchg16 (short *x, short y);
;
; Atomically assigns the 16-bit value y to *x and returns
; the original (before assignment) 16-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 4
                public ___rw_atomic_xchg16
___rw_atomic_xchg16 proc                    ; short (short *x, short y)

arg_x           = dword ptr  4
arg_y           = word ptr  8
                                            
                mov     ecx, [esp+arg_x]    ; %ecx = x                 
                mov     ax,  [esp+arg_y]    ; %eax = y                 
                xchg    ax,  [ecx]          ; %ax <-> (%ecx)           
                ret
___rw_atomic_xchg16 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int __rw_atomic_xchg32 (int *x, int y);
;
; Atomically assigns the 32-bit value y to *x and returns
; the original (before assignment) 32-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 4
                public ___rw_atomic_xchg32
___rw_atomic_xchg32 proc                    ; int (int *x, int y)

arg_x           = dword ptr  4
arg_y           = dword ptr  8
                                            
                mov     ecx, [esp+arg_x]    ; %ecx = x
                mov     eax, [esp+arg_y]    ; %eax = y
                xchg    eax, [ecx]          ; %eax <-> (%ecx)
                ret
___rw_atomic_xchg32 endp



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" char __rw_atomic_add8 (char *x, int y);
;
; Atomically increments the 8-bit value *x by y and returns
; the new (after increment) 8-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 4
                public ___rw_atomic_add8
___rw_atomic_add8 proc                       ; char (char *dst, int inc)

arg_dst         = dword ptr  4
arg_inc         = dword ptr  8
                                           
                mov     ecx, [esp+arg_dst]   ; %ecx = dst               
                mov     eax, [esp+arg_inc]   ; %eax = inc               
                mov     edx, eax           

                lock xadd [ecx], al          ; tmp = *dst;
                                             ; dst += inc;
                                             ; %al = tmp                

                add     eax, edx             ; return %eax + inc        
                ret
___rw_atomic_add8 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" short __rw_atomic_add16 (short *x, short y);
;
; Atomically increments the 16-bit value *x by y and returns
; the new (after increment) 16-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 4
                public ___rw_atomic_add16
___rw_atomic_add16 proc                      ; short (short *dst, short inc)

arg_dst         = dword ptr  4
arg_inc         = dword ptr  8

                mov     ecx, [esp+arg_dst]   ; %ecx = dst               
                mov     eax, [esp+arg_inc]   ; %eax = inc               
                mov     edx, eax           
                                          
                lock xadd [ecx], ax          ; tmp = *dst;
                                             ; dst += inc;
                                             ; %ax = tmp                
                                          
                add     eax, edx             ; return %eax + inc        
                ret
___rw_atomic_add16 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; extern "C" int __rw_atomic_add32 (int *x, int y);
;
; Atomically increments the 32-bit value *x by y and returns
; the new (after increment) 32-bit value of *x.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

                align 4
                public ___rw_atomic_add32
___rw_atomic_add32 proc                      ; int (int *dst, int inc)

arg_dst         = dword ptr  4
arg_inc         = dword ptr  8

                mov     ecx, [esp+arg_dst]   ; %ecx = dst               
                mov     edx, [esp+arg_inc]   ; %edx = inc               
                mov     eax, edx         
                                        
                lock xadd [ecx], eax         ; tmp = *dst;
                                             ; dst += inc;
                                             ; %eax = tmp                
                                        
                add     eax, edx             ; return %eax + inc        
                ret
___rw_atomic_add32 endp
