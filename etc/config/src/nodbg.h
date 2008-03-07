// definition of the nodbg() function

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
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
 * Copyright 1999-2008 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#ifdef _WIN32

#  include <windows.h>

#  ifdef _MSC_VER
#    include <crtdbg.h>  // for _CrtSetReportMode()
#  endif

void nodbg ()
{
#  ifdef _MSC_VER
    // prevent the Microsoft C library from popping up GUI windows
    // on various failures and direct error messages to the console
    // instead
    _CrtSetReportMode (_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#  endif

    // disable displaying the critical-error-handler
    // and general-protection-fault message boxes
    // windows.h included in thread.h
    SetErrorMode (SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
}

#else

void nodbg () { }

#endif   // _WIN32
