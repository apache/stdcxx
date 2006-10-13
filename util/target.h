/************************************************************************
 *
 * target.h - Struct definitions for target execution and results.
 *
 * $Id$
 *
 ************************************************************************
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
 **************************************************************************/

#ifndef RW_TARGET_H
#define RW_TARGET_H

#if !defined (_WIN32) && !defined (_WIN64)
#  include <unistd.h> /* For _XOPEN_UNIX */
#endif

#ifdef _XOPEN_UNIX
#  include <sys/resource.h> /* for struct rlimit */
#  include <sys/time.h> /* for struct timeval */
/**
   Abstraction typedef for struct rlimit using real struct
*/
typedef struct rlimit rw_rlimit;
/**
   Abstraction typedef for struct timeval using real struct
*/
typedef struct timeval rw_timeval;
#else /* _XOPEN_UNIX */
/**
   Placeholder rlim_t for use in rw_rlimit
*/
typedef unsigned long rw_rlim_t;
/**
   Placeholder struct rlimit to use if _XOPEN_UNIX isn't defined
*/
struct rw_rlimit {
    rw_rlim_t rlim_cur;
    rw_rlim_t rlim_max;
};
/**
   Abstraction typedef for struct rlimit using placeholder struct
*/
typedef struct rw_rlimit rw_rlimit;
/**
   Placeholder time_t for use in rw_timeval
*/
typedef long rw_time_t;
/**
   Placeholder suseconds_t for use in rw_timeval
*/
typedef long rw_suseconds_t;
/**
   Placeholder struct timeval to use if _XOPEN_UNIX isn't defined
*/
struct rw_timeval {
    rw_time_t tv_sec;
    rw_suseconds_t tv_usec;
};
/**
   Abstraction typedef for struct timeval using placeholder struct
*/
typedef struct rw_timeval rw_timeval;
#endif /* _XOPEN_UNIX */

#ifndef RLIM_INFINITY
#  define RLIM_INFINITY -1
#endif /* RLIM_INFINITY */

#ifndef RLIM_SAVED_CUR
#  define RLIM_SAVED_CUR RLIM_INFINITY
#endif   /* RLIM_SAVED_CUR */

#ifndef RLIM_SAVED_MAX
#  define RLIM_SAVED_MAX RLIM_INFINITY
#endif   /* RLIM_SAVED_MAX */

/**
   Structure encapsulating the options involved in executing a run.
*/
struct target_opts {
    char** argv; /**< Target argv array. */
    unsigned timeout; /**< Allowed time for process execution. */
    const char* data_dir; /**< Root dir for reference input/output files. */
    const char* c_warn; /**< Warning flag string when compiling. */
    const char* l_warn; /**< Warning flag string when linking. */
    int compat; /**< Test suite compatability mode switch. */
    rw_rlimit* core;
    rw_rlimit* cpu;
    rw_rlimit* data;
    rw_rlimit* fsize;
    rw_rlimit* nofile;
    rw_rlimit* stack;
    rw_rlimit* as;
};

/**
   Status codes to denote result of analysis.
*/
enum ProcessStatus {
    ST_OK = 0, /**< Default (OK) status */
    ST_COMPILE, /**< Target failed to compile */
    ST_LINK, /**< Target failed to link */
    ST_EXIST, /**< Target doesn't exist? */
    ST_EXECUTE_FLAG, /**< Target lacks X flag */
    ST_EXECUTE, /**< Target failed to execute? */
    ST_NO_OUTPUT, /**< Target produced no output */
    ST_NO_REF, /**< No reference file found */
    ST_BAD_REF, /**< Invalid reference file found */
    ST_BAD_OUTPUT, /**< Incorrect output found */
    ST_FORMAT, /**< Incorrectly formatted (test) output found */
    ST_OVERFLOW, /**< Assertion counter overflow */
    ST_SYSTEM_ERROR, /**< System error during file system operation */
    ST_KILLED, /**< Target killed by exec utility */
    ST_NOT_KILLED, /** Target not killed by exec utility */
    ST_LAST /**< Array terminator */
};

/**
   Structure encapsulating the results extracted from a run.
*/
struct target_status {
    int exit; /**< exit/signal code for process. */
    int signaled; /**< 1 if exit is a signal number, 0 denoting exit code 
                     otherwise */
    enum ProcessStatus status; /**< Textual process status. */
    const rw_timeval* user; /**< Elapsed user time spent in execution. */
    const rw_timeval* sys; /**< Elapsed system time spent in execution. */
    const rw_timeval* wall; /**< Wall clock time spent in execution. */
    unsigned c_warn; /**< Number of compile warnings. */
    unsigned l_warn; /**< Number of link warnings. */
    unsigned t_warn; /**< Number of (test) warnings. */
    unsigned assert; /**< Number of (test) assertions. */
    unsigned failed; /**< Number of failed (test) assertions. */
};

#endif   /* RW_TARGET_H */
