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

// disable Compaq/HP C++ pure libc headers to allow POSIX symbols
// such as SIGALRM or SIGKILL to be defined
#undef __PURE_CNAME

#include <assert.h> /* for assert */
#include <ctype.h> /* for tolower */
#include <errno.h> /* for errno */
#include <fcntl.h> /* for O_*, */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for str*, mem* */

#include <unistd.h> /* for close, dup, exec, fork */
#include <sys/stat.h> /* for S_* */
#include <sys/types.h>
#include <sys/wait.h>

#include "cmdopt.h"
#include "util.h"

#include "exec.h"

/**
   Status flag used to comunicate that an alarm has triggered.

   Value is 0 when alarm hasn't been triggered or has been handled
   Value is 1 when alarm has been triggered and hasn't been handled

   @see handle_alrm
   @see open_input
*/
static int alarm_timeout;

/**
    Utility macro to generate a signal number/name pair.
    
    @parm val 'short' signal name (no leading SIG) to generate pair for.
    @see signal_names []
*/
#undef SIGNAL
#define SIGNAL(val)   { SIG ## val, #val }

/**
    Signal name/number translation table.
    
    This table is populated using the SIGNAL helper macro to translate system SIG* macro values into name/value pairs.
    
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
   Compare two characters in a case-insensitive manner

   @param c1 first character to compare
   @param c2 second character to compare
   @return an integer less than, equal to, or greater than 0, coresponding
   to whether c1 is less than, equal to, or greater than c2 when compared
   in a case insensitive manner.
*/
static int
rw_charcasecmp (char c1, char c2)
{
    typedef unsigned char UChar; 
    return tolower ((UChar)c1) - tolower ((UChar)c2);
}

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

    assert (0 != s1);
    assert (0 != s2);

    for (delta = rw_charcasecmp (*s1, *s2); 
         *s1 && *s2 && 0 == delta; 
         delta = rw_charcasecmp (*(++s1), *(++s2)));
    return delta;
}

/**
   Translates a human understandable signal name into a number usable by kill ().

   This method understands several formats for signal names.  They are as follows:
   - n
   - SIGFOO
   - FOO
   In this list, n denotes a number and FOO denotes a short signal name.

   @param signame a signal name to decode
   @returns the signal number or -1 if a number couldn't be determined
   @see signal_names []
*/
const int
get_signo (const char* signame)
{
    size_t i;
    typedef unsigned char UChar; 

    assert (0 != signame);

    if (isdigit (signame [0])){
        char *junk;
        int trans = strtol (signame, &junk, 10);

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

/**
   Translates a signal number into a human understandable name

   @param signo a signal number
   @returns the human understandable name for the signal (minus the SIG 
   prefix), or "#n" if the the name for the number is unknown to the 
   function, where n is signo
   @see signal_names []
*/
const char* 
get_signame (int signo)
{
    size_t i;
    static char def [16];

    for (i = 0; signal_names [i].str; ++i) {
        if (signal_names [i].val == signo) {
            return signal_names [i].str;
        }
    }

    /* We've run out of known signal numbers, so use a default name */
    snprintf (def, sizeof def, "SIG#%d", signo);
    return def;
}

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
static struct exec_attrs
wait_for_child (pid_t child_pid)
{
    static const int signals [] = {
        SIGHUP, SIGINT, SIGTERM, SIGKILL, SIGKILL
    };

    static const unsigned sigcount = sizeof (signals) / sizeof (int);

    struct sigaction act;

    struct exec_attrs state = {-1, 0};

    unsigned siginx = 0;

    int stopped = 0;

#ifdef WCONTINUED
    int waitopts = WUNTRACED | WCONTINUED;
#else
    int waitopts = WUNTRACED;
#endif
    assert (1 < child_pid);

    /* Clear timeout */
    alarm_timeout = 0;

    /* Set handler (if needed).  Need to use sigaction rather than signal due
       to linux glitch
    */
    memset (&act, 0, sizeof act);
    act.sa_handler = handle_alrm;
    sigaction (SIGALRM, &act, 0);
    
    if (timeout > 0)
        alarm (timeout);

    while (1) {
        const pid_t wait_pid = waitpid (child_pid, &state.status, waitopts);
        if (child_pid == wait_pid) {
            if (WIFEXITED (state.status) || WIFSIGNALED (state.status))
                break; /*we've got an exit state, so let's bail*/
            else if (WIFSTOPPED (state.status))
                stopped = state.status;
#ifdef WIFCONTINUED /*Perhaps we should guard WIFSTOPPED with this guard also */
            else if (WIFCONTINUED (state.status))
                stopped = 0;
#endif
            else
                ;   /* huh? */
        }
        else if ((pid_t)-1 == wait_pid) {
            if (EINTR == errno && alarm_timeout) {
                /* timeout elapsed, so send a signal to the child */
                if (stopped) {
                    /* If the child process is stopped, it is incapable of
                       recieving signals.  Therefore, we'll record this
                       and break out of the loop.
                    */
                    state.status = stopped;
                    break;
                }

                /* ignore kill errors (perhaps should record them)*/
                (void)kill (child_pid, signals [siginx]);

                /* Record the signal used*/
                state.killed = signals [siginx];

                ++siginx;

                /* Step to the next signal */
                if (siginx > sigcount) {
                    /* Still running, but we've run out of signals to try
                       Therefore, we'll set error flags and break out of 
                       the loop.
                    */
                    state.status = -1;
                    state.killed = -1;
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
                else
                    ; /* Now what? */
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

    return state;
}

/**
   Opens an input file, based on exec_name

   Takes an executable name, and tries to open an input file based on this 
   name and the value of the in_root global variable.  If a file is found in 
   neither of two locattions derived from these variables, this method tries
   to fall back on /dev/null.

   Source file locations:
     - [in_root]/manual/in/[exec_name].in
     - [in_root]/tutorial/in/[exec_name].in
     - /dev/null

   @param exec_name the name of executable being run
   @returns the file descriptor of the opened file
   @see in_root
*/
static int
open_input (const char* exec_name)
{
    const size_t root_len = strlen (in_root);
    int intermit = -1;

    assert (0 != exec_name);
    assert (0 != in_root);

    if (root_len) {
        const size_t out_len = root_len + strlen (exec_name) + 17;
    
        char* const tmp_name = (char*)RW_MALLOC (out_len);

        /* Try in_root/manual/in/exec_name.in */
        memcpy (tmp_name, in_root, root_len+1);
        strcat (tmp_name, "/manual/in/");
        strcat (tmp_name, exec_name);
        strcat (tmp_name, ".in");
        intermit = open (tmp_name, O_RDONLY);
    
        /* If we opened the file, return the descriptor */
        if (0 <= intermit) {
            free (tmp_name);
            return intermit;
        }

        /* If the file exists (errno isn't ENOENT), exit */
        if (ENOENT != errno)
            terminate (1, "open (%s) failed: %s\n", tmp_name, 
                       strerror (errno));

        /* Try in_root/tutorial/in/exec_name.in */
        memcpy (tmp_name, in_root, root_len+1);
        strcat (tmp_name, "/tutorial/in/");
        strcat (tmp_name, exec_name);
        strcat (tmp_name, ".in");
        intermit = open (tmp_name, O_RDONLY);

        /* If we opened the file, return the descriptor */
        if (0 <= intermit) {
            free (tmp_name);
            return intermit;
        }

        /* If the file exists (errno isn't ENOENT), exit */
        if (-1 == intermit && ENOENT != errno)
            terminate (1, "open (%s) failed: %s\n", tmp_name, 
                       strerror (errno));

        free (tmp_name);
    }

    /* If we didn't find a source file, open /dev/null */

    intermit = open ("/dev/null", O_RDONLY);

    /* If we opened the file, return the descriptor */
    if (0 <= intermit)
        return intermit;

    /* otherwise, print an error message and exit */
    terminate (1, "open (/dev/null) failed: %s\n", strerror (errno));
    return -1; /* silence a compiler warning */
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

/**
   Entry point to the child process (watchdog) subsystem.

   This method fork ()s, creating a child process.  This child process becomes
   a process group leader, redirects input and output files, then exec ()s
   the target specified in argv [0], providing it with argv to use as its argv 
   array.  Meanwhile, the parent process calls the wait_for_child method to
   monitor the child process, passing the return value back to the calling
   method.

   @param exec_name name of the child executable
   @param argv argv array for child process
   @return structure describing how the child procees exited
   @see wait_for_child ()
*/
struct exec_attrs 
exec_file (char** argv)
{
    const pid_t child_pid = fork ();

    if (0 == child_pid) {   /* child */
        FILE* error_file;

        assert (0 != argv);
        assert (0 != argv [0]);
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
            const int intermit = open_input (target_name);
            replace_file (intermit, 0, "stdin");
        }

        /* Redirect stdout */
        {
            const size_t exelen = strlen (argv [0]);
            const size_t outlen = exelen + 5;
            char* const tmp_name = (char*)RW_MALLOC (outlen);
            int intermit;

            /* Redirect stdout */
            memcpy (tmp_name, argv [0], exelen + 1);
            strcat (tmp_name, ".out");
            intermit = open (tmp_name, O_WRONLY | O_CREAT | O_TRUNC, 
                             S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

            if (-1 == intermit)
                terminate (1, "Error opening %s for output redirection: "
                           "%s\n", tmp_name, strerror (errno));

            replace_file (intermit, 1, "stdout");

            free (tmp_name);
        }

        /* Redirect stderr */
        if (-1 == dup2 (1, 2))
            terminate (1, "Redirection of stderr to stdout failed: %s\n", 
                       strerror (errno));

        execv (argv [0], argv);

        fprintf (error_file, "%s (%s): execv (\"%s\", ...) error: %s\n",
                 exe_name, target_name, argv [0], strerror (errno));

        exit (1);
    }

    if (-1 == child_pid) {
        /* Fake a failue to execute status return structure */
        struct exec_attrs state = {127, -1};
        warn ("Unable to create child process for %s: %s\n", argv [0],
              strerror (errno));
        return state;
    }
    
    /* parent */
    return wait_for_child (child_pid);
}
