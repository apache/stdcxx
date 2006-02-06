/************************************************************************
 *
 * alarm.h - declaration of rw_alarm() and other helpers
 *
 * $Id$
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

#ifndef RW_ALARM_H_INCLUDED
#define RW_ALARM_H_INCLUDED


#include <testdefs.h>


extern "C" {

typedef void rw_signal_handler_t (int);

extern _TEST_EXPORT rw_signal_handler_t* const rw_sig_dfl;
extern _TEST_EXPORT rw_signal_handler_t* const rw_sig_ign;
extern _TEST_EXPORT rw_signal_handler_t* const rw_sig_hold;
extern _TEST_EXPORT rw_signal_handler_t* const rw_sig_restore;

}

_TEST_EXPORT unsigned
rw_alarm (unsigned int, rw_signal_handler_t* = 0);


#endif   // RW_ALARM_H_INCLUDED
