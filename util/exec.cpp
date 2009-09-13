/************************************************************************
 *
 * exec.cpp - Definitions of the child process subsystem
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

/* disable Compaq/HP C++ pure libc headers to allow POSIX symbols
   such as SIGALRM or SIGKILL to be defined. */
#undef __PURE_CNAME

#include <assert.h> /* for assert */
#include <ctype.h> /* for tolower */
#include <errno.h> /* for errno */
#include <fcntl.h> /* for O_*, */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for str*, mem* */

#ifndef _WIN32
#  include <unistd.h> /* for close, dup, exec, fork */
#  include <sys/wait.h>
#  include <sys/times.h> /* for times - is this XSI? */
#  ifdef _XOPEN_UNIX
#    include <sys/resource.h> /* for setlimit(), RLIMIT_CORE, ... */
#  endif
#else
#  ifndef _WIN32_WINNT
#    define _WIN32_WINNT 0x0500
#  endif
#  include <windows.h> /* for PROCESS_INFORMATION, CreateProcess, ... */
#  ifndef SIGTRAP
#    define SIGTRAP   5   // STATUS_BREAKPOINT translated into SIGTRAP
#  endif
#  ifndef SIGBUS
#    define SIGBUS    10  // STATUS_IN_PAGE_ERROR translated into SIGBUS
#  endif
#  ifndef SIGSYS
#    define SIGSYS    12  // STATUS_INVALID_PARAMETER translated into SIGSYS
#  endif
#  ifndef SIGSTKFLT
#    define SIGSTKFLT 16  // STATUS_FLOAT_STACK_CHECK translated into SIGSTKFLT
#  endif
#  ifndef STATUS_INVALID_PARAMETER
#    define STATUS_INVALID_PARAMETER          ((DWORD)0xC000000DL)
#  endif
#  ifndef STATUS_HEAP_CORRUPTION
#    define STATUS_HEAP_CORRUPTION            ((DWORD)0xC0000374L)
#  endif
#  ifndef STATUS_STACK_BUFFER_OVERRUN
#    define STATUS_STACK_BUFFER_OVERRUN       ((DWORD)0xC0000409L)
#  endif
#  ifndef STATUS_INVALID_CRUNTIME_PARAMETER
#    define STATUS_INVALID_CRUNTIME_PARAMETER ((DWORD)0xC0000417L)
#  endif
#endif

#ifndef SIGHUP
#  define SIGHUP     1   /* Linux value */
#endif
#ifndef SIGQUIT
#  define SIGQUIT    3   /* Linux value */
#endif
#ifndef SIGKILL
#  define SIGKILL    9   /* Linux value */
#endif
#ifndef SIGALRM
#  define SIGALRM   14   /* Linux value */
#endif


#ifndef ENOENT
#  define ENOENT     1   /* Linux value */
#endif
#ifndef ESRCH
#  define ESRCH      3   /* Linux value */
#endif
#ifndef EINTR
#  define EINTR      4   /* Linux value */
#endif
#ifndef ECHILD
#  define ECHILD    10   /* Linux value */
#endif
#ifndef EINVAL
#  define EINVAL    22   /* Linux value */
#endif

#include <sys/stat.h> /* for S_* */
#include <sys/types.h>

#include "cmdopt.h"
#include "target.h" /* For struct target_opts */
#include "util.h"

#include "exec.h"

#if !defined (_WIN32) && !defined (_RWSTD_NO_PURE_C_HEADERS)
#  ifdef __cplusplus
extern "C" {
#  endif

int kill (pid_t pid, int sig);

FILE* fdopen (int fd, const char *mode);

#  ifdef __cplusplus
}   /* extern "C" */
#  endif
#endif   /* !_WIN32 && !_RWSTD_NO_PURE_C_HEADERS */


/**
   Status flag used to comunicate that an alarm has triggered.

   Value is 0 when alarm hasn't been triggered or has been handled
   Value is 1 when alarm has been triggered and hasn't been handled

   @see handle_alrm
   @see handle_term_signal
*/
static int alarm_timeout;

/**
   Record of fatal signal recieved.  Used to raise() the signal again after
   child process has been killed.

   @see handle_term_signal
*/
static int kill_signal;

/**
    Utility macro to generate a signal number/name pair.
    
    @parm val 'short' signal name (no leading SIG) to generate pair for.
    @see signal_names []
*/
#undef SIGNAL
#define SIGNAL(val)   { SIG ## val, #val }

/**
    Signal name/number translation table.
    
    This table is populated using the SIGNAL helper macro to translate
    system SIG* macro values into name/value pairs.
    
    @see SIGNAL ()
*/
static const struct {
    int         val; /**< Signal value for lookup pair */
    const char* str; /**< Signal name for lookup pair */
} signal_names [] = {
#ifdef SIGABRT
    SIGNAL (ABRT),
#endif   /* SIGABRT */
#ifdef SIGALRM
    SIGNAL (ALRM),
#endif   /* SIGALRM */
#ifdef SIGBUS
    SIGNAL (BUS),
#endif   /* SIGBUS */
#ifdef SIGCANCEL
    SIGNAL (CANCEL),
#endif   /* SIGCANCEL */
#ifdef SIGCHLD
    SIGNAL (CHLD),
#endif   /* SIGCHLD */
#ifdef SIGCKPT
    SIGNAL (CKPT),
#endif   /* SIGCKPT */
#ifdef SIGCLD
    SIGNAL (CLD),
#endif   /* SIGCLD */
#ifdef SIGCONT
    SIGNAL (CONT),
#endif   /* SIGCONT */
#ifdef SIGDIL
    SIGNAL (DIL),
#endif   /* SIGDIL */
#ifdef SIGEMT
    SIGNAL (EMT),
#endif   /* SIGEMT */
#ifdef SIGFPE
    SIGNAL (FPE),
#endif   /* SIGFPE */
#ifdef SIGFREEZE
    SIGNAL (FREEZE),
#endif   /* SIGFREEZE */
#ifdef SIGGFAULT
    SIGNAL (GFAULT),
#endif   /* SIGGFAULT */
#ifdef SIGHUP
    SIGNAL (HUP),
#endif   /* SIGHUP */
#ifdef SIGILL
    SIGNAL (ILL),
#endif   /* SIGILL */
#ifdef SIGINFO
    SIGNAL (INFO),
#endif   /* SIGINFO */
#ifdef SIGINT
    SIGNAL (INT),
#endif   /* SIGINT */
#ifdef SIGIO
    SIGNAL (IO),
#endif   /* SIGIO */
#ifdef SIGIOT
    SIGNAL (IOT),
#endif   /* SIGIOT */
#ifdef SIGK32
    SIGNAL (K32),
#endif   /* SIGK32 */
#ifdef SIGKILL
    SIGNAL (KILL),
#endif   /* SIGKILL */
#ifdef SIGLOST
    SIGNAL (LOST),
#endif   /* SIGLOST */
#ifdef SIGLWP
    SIGNAL (LWP),
#endif   /* SIGLWP */
#ifdef SIGPIPE
    SIGNAL (PIPE),
#endif   /* SIGPIPE */
#ifdef SIGPOLL
    SIGNAL (POLL),
#endif   /* SIGPOLL */
#ifdef SIGPROF
    SIGNAL (PROF),
#endif   /* SIGPROF */
#ifdef SIGPTINTR
    SIGNAL (PTINTR),
#endif   /* SIGPTINTR */
#ifdef SIGPTRESCHED
    SIGNAL (PTRESCHED),
#endif   /* SIGPTRESCHED */
#ifdef SIGPWR
    SIGNAL (PWR),
#endif   /* SIGPWR */
#ifdef SIGQUIT
    SIGNAL (QUIT),
#endif   /* SIGQUIT */
#ifdef SIGRESTART
    SIGNAL (RESTART),
#endif   /* SIGRESTART */
#ifdef SIGRESV
    SIGNAL (RESV),
#endif   /* SIGRESV */
#ifdef SIGSEGV
    SIGNAL (SEGV),
#endif   /* SIGSEGV */
#ifdef SIGSTKFLT
    SIGNAL (STKFLT),
#endif   /* SIGSTKFLT */
#ifdef SIGSTOP
    SIGNAL (STOP),
#endif   /* SIGSTOP */
#ifdef SIGSYS
    SIGNAL (SYS),
#endif   /* SIGSYS */
#ifdef SIGTERM
    SIGNAL (TERM),
#endif   /* SIGTERM */
#ifdef SIGTHAW
    SIGNAL (THAW),
#endif   /* SIGTHAW */
#ifdef SIGTRAP
    SIGNAL (TRAP),
#endif   /* SIGTRAP */
#ifdef SIGTSTP
    SIGNAL (TSTP),
#endif   /* SIGTSTP */
#ifdef SIGTTIN
    SIGNAL (TTIN),
#endif   /* SIGTTIN */
#ifdef SIGTTOU
    SIGNAL (TTOU),
#endif   /* SIGTTOU */
#ifdef SIGUNUSED
    SIGNAL (UNUSED),
#endif   /* SIGUNUSED */
#ifdef SIGURG
    SIGNAL (URG),
#endif   /* SIGURG */
#ifdef SIGUSR1
    SIGNAL (USR1),
#endif   /* SIGUSR1 */
#ifdef SIGUSR2
    SIGNAL (USR2),
#endif   /* SIGUSR2 */
#ifdef SIGVTALRM
    SIGNAL (VTALRM),
#endif   /* SIGVTALRM */
#ifdef SIGWAITING
    SIGNAL (WAITING),
#endif   /* SIGWAITING */
#ifdef SIGWINCH
    SIGNAL (WINCH),
#endif   /* SIGWINCH */
#ifdef SIGWINDOW
    SIGNAL (WINDOW),
#endif   /* SIGWINDOW */
#ifdef SIGXCPU
    SIGNAL (XCPU),
#endif   /* SIGXCPU */
#ifdef SIGXFSZ
    SIGNAL (XFSZ),
#endif   /* SIGXFSZ */
#ifdef SIGXRES
    SIGNAL (XRES),
#endif   /* SIGXRES */
    { -1, 0 }
};

/**
   Reimplementation of the POSIX strcasecmp function.

   This is a simplistic (re)implementation of the strcasecmp function
   specified in the XSI extension to the IEEE Std 1003.1 (POSIX) standard.

   @param s1 pointer to first string to compare
   @param s2 pointer to second string to compare
   @return an integer less than, equal to, or greater than 0, coresponding
   to whether s1 is less than, equal to, or greater than s2 when compared
   in a case insensitive manner.
*/
static int
rw_strcasecmp (const char* s1, const char* s2)
{
    int delta;

    typedef unsigned char UChar;

    assert (0 != s1);
    assert (0 != s2);

    do {
        delta = tolower ((UChar)*s1) - tolower ((UChar)*s2);
    } while (*s1++ && *s2++ && 0 == delta);

    return delta;
}


int
get_signo (const char* signame)
{
    size_t i;
    typedef unsigned char UChar; 

    assert (0 != signame);

    if (isdigit (signame [0])) {
        char *junk;
        int trans = int (strtol (signame, &junk, 10));

        if (0 == *junk && 0 == errno)
            return trans;
        else
            return -1;
    }

    if (   's' == tolower ((UChar)signame [0]) 
        && 'i' == tolower ((UChar)signame [1]) 
        && 'g' == tolower ((UChar)signame [2]))
        signame += 3;
    
    for (i = 0; signal_names [i].str; ++i) {
        if (0 == rw_strcasecmp (signal_names [i].str, signame)) {
            return signal_names [i].val;
        }
    }
    
    return -1;
}

const char* 
get_signame (int signo)
{
    size_t i;
    static char def [32];

    for (i = 0; signal_names [i].str; ++i) {
        if (signal_names [i].val == signo) {
            return signal_names [i].str;
        }
    }

    /* We've run out of known signal numbers, so use a default name */
    sprintf (def, "SIG#%d", signo);
    return def;
}

#ifndef _WIN32
/**
   Callback used to set the alarm_timeout flag in response to recieving
   the signal SIGALRM

   @param signo the signal recieved (should be SIGALRM)
   @see alarm_timeout
*/
#ifdef __cplusplus
extern "C" {
#endif

static void
handle_alrm (int signo)
{
    if (SIGALRM == signo)
        alarm_timeout = 1;
}

/**
   Callback used to gracefully terminate the utility if signaled to do so
   while running a target

   @param signo the signal recieved (should be in {SIGHUP, SIGINT, SIGQUIT, 
   SIGTERM})
   @see alarm_timeout

*/
static void
handle_term_signal (int signo)
{
    kill_signal = signo;
    alarm_timeout = 1;
}

typedef void (*alarm_handler)(int);

#ifdef __cplusplus
}
#endif

/**
   Monitors child_pid and determines how/when terminated.

   This method is built around a waitpid loop, which is responsible for
   determining the status of child_pid.  This loop breaks when we've detected
   that the child process has terminated, or when we conclude it won't 
   terminate.  To prevent the child process from running forever, we set 
   handle_alrm as the handler for SIGALRM using the sigaction system call and 
   set a timeout using the alarm () system call.  If this timeout expires, we 
   try to kill the child process using several different signals.

   @todo add better handling for corner conditions

   @param child_pid process ID number for the child process to monitor
   @return structure describing how the child procees exited
   @see handle_alrm
   @see alarm_timeout
*/
static void
wait_for_child (pid_t child_pid, int timeout, struct target_status* result)
{
    /* note that processes with no controlling terminal ignore
       the SIGINT and SIGQUIT signals
    */
    static const int signals [] = {
        SIGHUP, SIGINT, SIGQUIT, SIGTERM, SIGKILL, SIGKILL
    };

    static const unsigned sigcount = sizeof (signals) / sizeof (int);

    unsigned siginx = 0;

    int stopped = 0;

#ifdef WCONTINUED
    int waitopts = WUNTRACED | WCONTINUED;
#else
    int waitopts = WUNTRACED;
#endif

    int status;

    assert (1 < child_pid);

    /* Clear timeout */
    alarm_timeout = 0;

    /* Set handler (if needed).
    */
    rw_signal (SIGALRM, handle_alrm);
    
    /* Set handlers for SIGHUP, SIGINT, SIGQUIT, SIGTERM so we can kill the
       child process prior to dieing.
    */
    kill_signal = 0;

    rw_signal (SIGHUP, handle_term_signal);
    rw_signal (SIGINT, handle_term_signal);
    rw_signal (SIGQUIT, handle_term_signal);
    rw_signal (SIGTERM, handle_term_signal);

    if (timeout > 0)
        alarm (timeout);

    while (1) {
        const pid_t wait_pid = waitpid (child_pid, &status, waitopts);
        if (child_pid == wait_pid) {
            if (WIFEXITED (status)) {
                result->exit = WEXITSTATUS (status);

                /* from POSIX, 2.8.2 Exit Status for Commands:
                 *
                 * If a command is not found, the exit status shall be 127.
                 * If the command name is found, but it is not an executable
                 * utility, the exit status shall be 126. Applications that
                 * invoke utilities without using the shell should use these
                 * exit status values to report similar errors.
                 */
                switch (result->exit) {
                case 126:
                    result->status = ST_EXECUTE;
                    break;
                case 127:
                    result->status = ST_EXIST;
                    break;
                }
                break; /*we've got an exit state, so let's bail*/
            }
            else if (WIFSIGNALED (status)) {
                result->exit = WTERMSIG (status);
                result->signaled = 1;
                break; /*we've got an exit state, so let's bail*/
            }
            else if (WIFSTOPPED (status))
                stopped = status;
#ifdef WIFCONTINUED /*Perhaps we should guard WIFSTOPPED with this guard also */
            else if (WIFCONTINUED (status))
                stopped = 0;
#endif
            else {
                /* huh? */
            }
        }
        else if ((pid_t)-1 == wait_pid) {
            if (EINTR == errno && alarm_timeout) {
                /* timeout elapsed, so send a signal to the child */
                if (stopped) {
                    /* If the child process is stopped, it is incapable of
                       recieving signals.  Therefore, we'll record this
                       and break out of the loop.
                    */
                    result->exit = WTERMSIG (stopped);
                    result->signaled = 1;
                    break;
                }

                if (0 != kill (-child_pid, signals [siginx])) {
                    if (ESRCH == errno)
                        /* ESRCH means 'No process (group) found'.  Since 
                           there aren't any processes in the process group, 
                           we'll continue so we can collect the return value
                           if needed.
                        */
                        continue;
                    /* In addition to ESRCH, kill () may also set errno to 
                       EINVAL or EPERM, according to the POSIX spec, in 
                       addition to any platform specific extensions.
                       EPERM means 'No permissions to signal any recieving 
                       process'.  It is unlikely that this situation will
                       change, but we will try the remaining signals in the
                       signals array, in the same manner as if the signal had
                       been sent correctly.
                       EINVAL means 'The signal is an invalid or unsupported
                       signal number'.  As the signal number macros used in 
                       the signal array are hard coded, issues should be 
                       detected at compile time, not run time.  This is not a
                       fatal situation, so the remainder of signals in the
                       signal array will be tried, as if this transmission
                       had been successfull.
                       The correct behavior for any platform-specific 
                       extensions needs to be evaluated, but we are treating
                       them like EPERM or EINVAL at this time. */
                }

                /* Consider recording the signal used here.*/

                ++siginx;

                /* Step to the next signal */
                if (siginx >= sigcount) {
                    /* Still running, but we've run out of signals to try
                       Therefore, we'll set error flags and break out of 
                       the loop.
                    */
                    result->status = ST_NOT_KILLED;
                    break;
                }

                /* Reset the alarm */
                alarm_timeout = 0;
                alarm (1);
            }
            else if (EINTR == errno && !alarm_timeout) {
                /* continue iterating */
            }
            else if (EINVAL == errno) {
                if (waitopts)
                    /* bad waitpid options, reset to 0 and try again */
                    waitopts = 0;
                else {
                    /* Now what? */
                }
            }
            else if (ECHILD == errno) {
                /* should not happen */
                warn ("waitpid (%d) error: %s\n", (int)child_pid, 
                      strerror (errno));
            }
            else {
                /* waitpid () error */
                warn ("waitpid (%d) error: %s\n", (int)child_pid, 
                      strerror (errno));
            }
        }
        else if ((pid_t)0 == wait_pid) {
            /* should not happen */
        }
        else {
            /* what the heck? */
        }
    }

    /* Clear alarm */
    alarm (0);

    /* Kill/cleanup any grandchildren. */
    /* On solaris, this logic tries to avoid the situation where grandchild
       process times are rolled into the timing of a later process */
    while (siginx < sigcount && 0 == kill (-child_pid, signals [siginx])) {
        ++siginx;
        sleep (1);
    }

    /* Check if we were signaled to quit. */
    if (kill_signal) {
        /* Reset the handlers to normal */
        rw_signal (SIGHUP, SIG_DFL);
        rw_signal (SIGINT, SIG_DFL);
        rw_signal (SIGQUIT, SIG_DFL);
        rw_signal (SIGTERM, SIG_DFL);

        if (0 > raise (kill_signal))
            terminate (1, "raise(%s) failed: %s\n",
                       get_signame (kill_signal), strerror (errno));
    }
}


/**
   Replaces one file descriptor with a second, closing second after replacing
   the first.

   @param source file descriptor to copy
   @param dest file descriptor to replace
   @param name human understanable name for dest, used in error messages
*/
static void
replace_file (int source, int dest, const char* name)
{
    int result;

    assert (source != dest);
    assert (0 <= source);
    assert (0 <= dest);
    assert (0 != name);

    result = dup2 (source, dest);
    if (-1 == result)
        terminate (1, "redirecting to %s failed: %s\n", name, 
                   strerror (errno));

    result = close (source);
    if (-1 == result)
        terminate (1, "closing source for %s redirection failed: %s\n", 
                   name, strerror (errno));
}

#ifdef _XOPEN_UNIX
/**
    Utility macro to generate an rlimit tuple.
    
    @parm val 'short' resource name (no leading RLIMIT_).
    @param idx rw_rlimit pointer in the target_opts structure
    @see limit_process
    @see target_opts
*/
#undef LIMIT
#define LIMIT(val, idx)   { RLIMIT_ ## val, options->idx, #val }

/**
   Set process resource limits, based on the child_limits global.

   This method uses the LIMIT macro to build an internal array of limits to 
   try setting.  If setrlimit fails, we print a warning message (into the 
   output file) and move on.
   @param options structure containing limits to set.
*/
static void
limit_process (const struct target_opts* options)
{
    static const struct {
        int resource;
        rw_rlimit* limit;
        const char* name;
    } limits[] = {
#ifdef RLIMIT_CORE
        LIMIT (CORE, core),
#endif   /* RLIMIT_CORE */
#ifdef RLIMIT_CPU
        LIMIT (CPU, cpu),
#endif   /* RLIMIT_CPU */
#ifdef RLIMIT_DATA
        LIMIT (DATA, data),
#endif   /* RLIMIT_DATA */
#ifdef RLIMIT_FSIZE
        LIMIT (FSIZE, fsize),
#endif   /* RLIMIT_FSIZE */
#ifdef RLIMIT_NOFILE
        LIMIT (NOFILE, nofile),
#endif   /* RLIMIT_NOFILE */
#ifdef RLIMIT_STACK
        LIMIT (STACK, stack),
#endif   /* RLIMIT_STACK */
#ifdef RLIMIT_AS
        LIMIT (AS, as),
#endif   /* RLIMIT_AS */    
        { 0, 0, 0 }
    };

    for (size_t i = 0; limits [i].name; ++i) {
        struct rlimit local;

        if (!limits [i].limit)
            continue;

        memcpy (&local, limits [i].limit, sizeof local);

        if (setrlimit (limits [i].resource, &local)) {
            warn ("error setting process limits for %s (soft: %lu, hard: "
                  "%lu): %s\n", limits [i].name, local.rlim_cur, 
                  local.rlim_max, strerror (errno));
        }
    }
}
#endif /* _XOPEN_UNIX */

/**
   Calculates the amount of resources used by the child processes.

   This method uses the times() system call to calculate the resources used 
   by the child process.  However, times() only is able to calcualte agragate 
   usage by all child processes, not usage by a specific child process.
   Therefore, we must keep a running tally of how much resources had been used
   the previous time we calculated the usage.  This difference is the resources
   that were used by the process that just completed.

   @param result target_status structure to populate with process usage.
   @param h_clk starting (wall clock) time
   @param h_tms starting (system/user) time
*/
static void
calculate_usage (struct target_status* result, const clock_t h_clk, 
                 const struct tms* const h_tms)
{
    struct tms c_tms;
    clock_t c_clk;

    assert (0 != result);
    assert (0 != h_tms);

    c_clk = times (&c_tms);

    if ((clock_t)-1 == c_clk) {
        warn ("Failed to retrieve ending times: %s", strerror (errno));
        return;
    }

    /* time calculations */
    result->wall_time = c_clk - h_clk;
    result->usr_time  = c_tms.tms_cutime - h_tms->tms_cutime;
    result->sys_time  = c_tms.tms_cstime - h_tms->tms_cstime;
}

void exec_file (const struct target_opts* options, struct target_status* result)
{
    const pid_t child_pid = fork ();

    assert (0 != options);
    assert (0 != options->argv);
    assert (0 != options->argv [0]);

    if (0 == child_pid) {   /* child */
        const char* const target_name = get_target ();
        FILE* error_file;

        assert (0 != target_name);

        /* Set process group ID (so entire group can be killed)*/
        {
            const pid_t pgroup = setsid ();
            if (getpid () != pgroup)
                terminate (1, "Error setting process group: %s\n",
                           strerror (errno));
        }

        /* Cache stdout for use if execv () fails */
        {
            const int error_cache = dup (2);
            if (-1 == error_cache)
                terminate (1, "Error duplicating stderr: %s\n", 
                           strerror (errno));

            error_file = fdopen (error_cache,"a");
            if (0 == error_file)
                terminate (1, "Error opening file handle  from cloned "
                           "stderr file descriptor: %s\n", strerror (errno));
        }

        /* Redirect stdin */
        {
            const int intermit = open (options->infname, O_RDONLY);
            replace_file (intermit, 0, "stdin");
        }

        /* Redirect stdout */
        {
            int intermit;

            intermit = open (options->outfname,
                             O_WRONLY | O_CREAT | O_TRUNC, 
                             S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

            if (-1 == intermit)
                terminate (1, "Error opening %s for output redirection: "
                           "%s\n", options->outfname, strerror (errno));

            replace_file (intermit, 1, "stdout");
        }

        /* Redirect stderr */
        if (-1 == dup2 (1, 2))
            terminate (1, "Redirection of stderr to stdout failed: %s\n", 
                       strerror (errno));

#ifdef _XOPEN_UNIX
        limit_process (options);
#endif /* _XOPEN_UNIX */

        execv (options->argv [0], options->argv);

        /* POSIX specifies status of 127 when the executable doesn't
         * exist and 126 for all other exec failures
         */
        if (ENOENT == errno)
            exit (127);

        exit (126);
    }

    if (-1 == child_pid) {
        result->status = ST_EXECUTE;
        warn ("Unable to create child process for %s: %s\n", options->argv [0],
              strerror (errno));
    }
    else {
        /* parent */
        struct tms h_tms;
        clock_t h_clk = times (&h_tms);
        wait_for_child (child_pid, options->timeout, result);
        if ((clock_t)-1 != h_clk)
            calculate_usage (result, h_clk, &h_tms);
        else
            warn ("Failed to retrieve start times: %s", strerror (errno));
    }
}
#else  /* _WIN32 */

// map between NT_STATUS value and corresponding UNIX signal
static const struct {
    DWORD nt_status;
    int   signal;
} nt_status_map [] = {
    { STATUS_BREAKPOINT,                 SIGTRAP   },
    { STATUS_ACCESS_VIOLATION,           SIGSEGV   },
    { STATUS_STACK_OVERFLOW,             SIGSEGV   },
    { STATUS_HEAP_CORRUPTION,            SIGSEGV   },
    { STATUS_STACK_BUFFER_OVERRUN,       SIGSEGV   },
    { STATUS_IN_PAGE_ERROR,              SIGBUS    },
    { STATUS_ILLEGAL_INSTRUCTION,        SIGILL    },
    { STATUS_PRIVILEGED_INSTRUCTION,     SIGILL    },
    { STATUS_FLOAT_DENORMAL_OPERAND,     SIGFPE    },
    { STATUS_FLOAT_DIVIDE_BY_ZERO,       SIGFPE    },
    { STATUS_FLOAT_INEXACT_RESULT,       SIGFPE    },
    { STATUS_FLOAT_INVALID_OPERATION,    SIGFPE    },
    { STATUS_FLOAT_OVERFLOW,             SIGFPE    },
    { STATUS_FLOAT_UNDERFLOW,            SIGFPE    },
    { STATUS_INTEGER_DIVIDE_BY_ZERO,     SIGFPE    },
    { STATUS_INTEGER_OVERFLOW,           SIGFPE    },
    { STATUS_FLOAT_STACK_CHECK,          SIGSTKFLT },
    { STATUS_INVALID_PARAMETER,          SIGSYS    },
    { STATUS_INVALID_CRUNTIME_PARAMETER, SIGSYS    }
};


/**
   Convert an argv array into a string that can be passed to CreateProcess.

   This method allocates memory which the caller is responsible for free ()ing.
   The provided argv array is converted into a series of quoted strings.

   @param argv argv array to convert
   @return allocated array with converted contents
*/
static char*
merge_argv (char** argv)
{
    size_t len = 0;
    char** opts;
    char* term;
    char* merge;
    char* pos;

    assert (0 != argv);

    for (opts = argv; *opts; ++opts) {
        len += 3; /* for open ", close " and trailing space or null */
        for (term = *opts; *term; ++term) {
            if ('"' == *term || escape_code == *term)
                ++len; /* Escape embedded "s and ^s*/
            ++len;
        }
    }

    pos = merge = (char*)RW_MALLOC (len);
    for (opts = argv; *opts; ++opts) {
        *(pos++) = '"';
        for (term = *opts; *term; ++term) {
            if ('"' == *term || escape_code == *term)
                *(pos++) = escape_code;  /* Escape embedded "s and ^s*/
            *(pos++) = *term;
        }
        *(pos++) = '"';
        *(pos++) = ' ';
    }
    *(pos-1) = '\0'; /* convert trailing space to null */
    return merge;
}

/**
   Wrapper function around warn for windows native API calls.

   @param action Human understandable description of failed action.
   @return Value of GetLastError.
*/
static DWORD
warn_last_error (const char* action)
{
    DWORD error = GetLastError (); 

    if (error) {
        LPTSTR error_text = 0;
        if (FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                           FORMAT_MESSAGE_FROM_SYSTEM, NULL, error,
                           MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPTSTR)&error_text, 0, NULL)) {
            warn ("%s failed with error %d: %s\n", action, error, error_text);
            LocalFree (error_text);
        }
        else {
            warn ("%s failed with error %d.  Additionally, FormatMessage "
                  "failed with error %d.\n", action, error, GetLastError ());
        }
    }
    return error;
}

/**
   Try killing the child process with what (limited) facilities we have at
   our disposal.  Does not collect exit code or close child process handle.

   @param process Process handle of process to kill.
   @param result status structure to record system errors in.
*/
static void
kill_child_process (PROCESS_INFORMATION child, struct target_status* result)
{
    OSVERSIONINFO OSVer;

    OSVer.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
    if (0 == GetVersionEx (&OSVer)) {
        result->status = ST_SYSTEM_ERROR;
        warn_last_error ("Retrieving host version");
        return;
    }
    /* Try to soft kill child process group if it didn't terminate, but only 
       on NT */

    if (VER_PLATFORM_WIN32_NT == OSVer.dwPlatformId) {

        struct sig_event
        {
            DWORD       signal_;
            const char* msg_;
        }
        sig_events [] = {
            { CTRL_C_EVENT,     "Sending child process Control-C"     },
            { CTRL_BREAK_EVENT, "Sending child process Control-Break" }
        };

        for (unsigned long i = 0;
            i < sizeof (sig_events) / sizeof (*sig_events); ++i) {
            DWORD wait_code;

            if (0 == GenerateConsoleCtrlEvent (sig_events [i].signal_,
                                               child.dwProcessId))
                warn_last_error (sig_events [i].msg_);

            wait_code = WaitForSingleObject (child.hProcess, 1000);
            if (WAIT_TIMEOUT == wait_code)
                continue;

            if (WAIT_OBJECT_0 != wait_code) {
                result->status = ST_SYSTEM_ERROR;
                warn_last_error ("Waiting for child process");
            }
            return;
        }
    }
    /* Then hard kill the child process */
    if (0 == TerminateProcess (child.hProcess, 3))
        warn_last_error ("Terminating child process");
    else if (WAIT_FAILED == WaitForSingleObject (child.hProcess, 1000))
        warn_last_error ("Waiting for child process");
}

/* FILETIME to ULONGLONG */
inline ULONGLONG fttoull (const FILETIME& ft)
{
    ULARGE_INTEGER __ft;
    __ft.LowPart  = ft.dwLowDateTime;
    __ft.HighPart = ft.dwHighDateTime;
    return __ft.QuadPart;
}

void exec_file (const struct target_opts* options, struct target_status* result)
{
    char* merged;
    PROCESS_INFORMATION child;
    STARTUPINFO context;
    SECURITY_ATTRIBUTES child_sa = /* SA for inheritable handle. */
          {sizeof (SECURITY_ATTRIBUTES), NULL, TRUE};
    DWORD real_timeout, wait_code;
    FILETIME start, end;

    assert (0 != options);
    assert (0 != options->argv);
    assert (0 != options->argv [0]);

    real_timeout = (options->timeout > 0) ? options->timeout * 1000 : INFINITE;

    /* Borrow our startup info */
    GetStartupInfo (&context);
    context.dwFlags = STARTF_USESTDHANDLES;

    /* Create I/O handles */
    {
        /* Output redirection */

        context.hStdOutput = CreateFile (options->outfname, GENERIC_WRITE, 
                FILE_SHARE_WRITE, &child_sa, CREATE_ALWAYS, 
                FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE == context.hStdOutput) { 
            result->status = ST_SYSTEM_ERROR;
            warn_last_error ("Opening child output stream");
            return;
        }

        context.hStdError = context.hStdOutput;

        /* Input redirection */
        context.hStdInput =
            CreateFile (options->infname, GENERIC_READ, FILE_SHARE_READ, 
                        &child_sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (INVALID_HANDLE_VALUE == context.hStdInput) { 
            CloseHandle (context.hStdOutput);
            result->status = ST_SYSTEM_ERROR;
            warn_last_error ("Opening child input stream");
            return;
        }
    }    

    merged = merge_argv (options->argv);

    /* set appropriate error mode (the child process inherits this
       error mode) to disable displaying the critical-error-handler
       and general-protection-fault message boxes */
    UINT old_mode = SetErrorMode (SEM_FAILCRITICALERRORS
                                | SEM_NOGPFAULTERRORBOX);

    /* Create the child process in suspended state */
    if (0 == CreateProcess (options->argv [0], merged, 0, 0, 1, 
        CREATE_NEW_PROCESS_GROUP | CREATE_SUSPENDED, 0, 0, &context, &child)) {
        /* record the status if we failed to create the process */
        result->status = ST_SYSTEM_ERROR;
        warn_last_error ("Creating child process");;
    }

    /* restore the previous error mode */
    SetErrorMode (old_mode);

    /* Clean up handles */
    if (context.hStdInput)
        if (0 == CloseHandle (context.hStdInput))
            warn_last_error ("Closing child input stream");
    if (0 == CloseHandle (context.hStdOutput))
        warn_last_error ("Closing child output stream");

    /* Clean up argument string*/
    free (merged);

    /* Return if we failed to create the child process */
    if (ST_SYSTEM_ERROR == result->status)
        return;

#if _WIN32_WINNT >= 0x0500
    if (options->as) {
        if (HANDLE hJob = CreateJobObject (NULL, NULL)) {
            if (AssignProcessToJobObject (hJob, child.hProcess)) {
                JOBOBJECT_EXTENDED_LIMIT_INFORMATION job_info = { 0 };
                
                job_info.BasicLimitInformation.LimitFlags =
                    JOB_OBJECT_LIMIT_PROCESS_MEMORY;
                
                const rw_rlimit* as = options->as;
                job_info.ProcessMemoryLimit =
                    as->rlim_cur < as->rlim_max ? as->rlim_cur : as->rlim_max;

                if (!SetInformationJobObject (hJob,
                                              JobObjectExtendedLimitInformation,
                                              &job_info, sizeof (job_info)))
                    warn_last_error ("Setting process limits");
            }
            else 
                warn_last_error ("Assigning process to job object");

            if (!CloseHandle (hJob))
                warn_last_error ("Closing job object handle");
        }
        else
            warn_last_error ("Creating job object");
    }
#endif   // _WIN32_WINNT >= 0x0500

    /* Check the wall clock before resuming the process */
    GetSystemTimeAsFileTime(&start);

    if (DWORD (-1) == ResumeThread (child.hThread))
        warn_last_error ("Resuming process");

    if (0 == CloseHandle (child.hThread))
        warn_last_error ("Closing child main thread handle");

    /* Wait for the child process to terminate */
    wait_code = WaitForSingleObject (child.hProcess, real_timeout);

    switch (wait_code) {
    case WAIT_OBJECT_0:
        break;
    case WAIT_TIMEOUT:
        /* Child process didn't shut down, so note that we killed it. */
        result->status = ST_KILLED;

        kill_child_process (child, result);
        break;
    default:
        result->status = ST_SYSTEM_ERROR;
        warn_last_error ("Waiting for child process");
    }

    /* Calculate wall clock time elapsed while the process ran */
    GetSystemTimeAsFileTime(&end);

    /* 100 nanosecond units in a second */
    const DWORD UNITS_PER_SEC = 10000000;
    const DWORD UNITS_PER_CLOCK = UNITS_PER_SEC / CLOCKS_PER_SEC;
    assert (UNITS_PER_CLOCK * CLOCKS_PER_SEC == UNITS_PER_SEC);

#if _WIN32_WINNT >= 0x0500
    FILETIME stime, utime;

    if (GetProcessTimes (child.hProcess, &start, &end, &stime, &utime)) {
        result->usr_time = clock_t (fttoull (utime) / UNITS_PER_CLOCK);
        result->sys_time = clock_t (fttoull (stime) / UNITS_PER_CLOCK);
    }
    else
        warn_last_error ("Getting child process times");
#endif  // _WIN32_WINNT >= 0x0500

    result->wall_time =
        clock_t ((fttoull (end) - fttoull (start)) / UNITS_PER_CLOCK);


    if (0 == GetExitCodeProcess (child.hProcess, (LPDWORD)&result->exit)) {
        warn_last_error ("Retrieving child process exit code");
        result->status = ST_SYSTEM_ERROR;
    }

    if (0 == CloseHandle (child.hProcess))
        warn_last_error ("Closing child process handle");

    for (int i = 0; i < sizeof (nt_status_map) / sizeof (*nt_status_map); ++i) {
        if (nt_status_map [i].nt_status == DWORD (result->exit)) {
            result->exit = nt_status_map [i].signal;
            result->signaled = 1;
            break;
        }
    }
}

#endif  /* _WIN32 */
