/************************************************************************
 *
 * driver.cpp - definitions of the test driver
 *
 * $Id$
 *
 ************************************************************************
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


// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC

#include <cmdopt.h>    // for rw_setopts()
#include <printf.h>    // for rw_sprintfa()

#include <assert.h>    // for assert
#include <setjmp.h>    // for longjmp, setjmp, ...
#include <stdarg.h>    // for va_list
#include <stdio.h>     // for fileno, fprintf
#include <stdlib.h>    // for free
#include <string.h>    // for strchr, strcpy

#if !defined (_WIN32) && !defined (_WIN64)
#  include <unistd.h>    // for isatty

// declare fileno in case it's not declared (for strict ANSI conformance)
extern "C" {

_RWSTD_DLLIMPORT int (fileno)(FILE*) _LIBC_THROWS ();

}   // extern "C"

#else
   // no isatty on Windoze
#  define _RWSTD_NO_ISATTY
#endif   // _WIN{32,64}

// expand _TEST_EXPORT macros
#define _RWSTD_TEST_SRC
#include <driver.h>

/************************************************************************/

#define RW_TEST_STRSTR(x)   #x
#define RW_TEST_STR(x)      RW_TEST_STRSTR(x)

#ifndef RW_TEST_COMPILER
#  if defined (__DECCXX__)
#    define RW_TEST_COMPILER "Compaq C++, __DECCXX__ = " \
            RW_TEST_STR (__DECCXX__)
#  elif defined (__INTEL_COMPILER)
#    if defined (__EDG_VERSION__)
#      define RW_TEST_ICC_EDG_VER \
              ", __EDG_VERSION__ = "  RW_TEST_STR (__EDG_VERSION__)
#    else
#      define RW_TEST_ICC_EDG_VER ""
#    endif
#    if defined (_MSC_VER)
#      define RW_TEST_COMPILER "Intel C++, __INTEL_COMPILER = " \
              RW_TEST_STR (__INTEL_COMPILER) ", _MSC_VER = " \
              RW_TEST_STR (_MSC_VER) \
              RW_TEST_ICC_EDG_VER
#    elif defined (__INTEL_COMPILER_BUILD_DATE)
#      define RW_TEST_COMPILER "Intel C++, __INTEL_COMPILER = " \
              RW_TEST_STR (__INTEL_COMPILER) \
              ", __INTEL_COMPILER_BUILD_DATE = " \
              RW_TEST_STR (__INTEL_COMPILER_BUILD_DATE) \
              RW_TEST_ICC_EDG_VER
#    else
#      define RW_TEST_COMPILER "Intel C++, __INTEL_COMPILER = " \
              RW_TEST_STR (__INTEL_COMPILER) \
              RW_TEST_ICC_EDG_VER
#    endif
#  elif defined (__GNUC__)
#    if defined (__VERSION__)
#      define RW_TEST_GCC_VER ", __VERSION__ = \"" __VERSION__ "\""
#    else
#      define RW_TEST_GCC_VER ""
#    endif
#    if defined (__GNUC_PATCHLEVEL__)
#      define RW_TEST_COMPILER "gcc "            \
              RW_TEST_STR (__GNUC__) "."         \
              RW_TEST_STR (__GNUC_MINOR__) "."   \
              RW_TEST_STR (__GNUC_PATCHLEVEL__)  \
              RW_TEST_GCC_VER
#    else
#      define RW_TEST_COMPILER "gcc " \
              RW_TEST_STR (__GNUC__) "." RW_TEST_STR (__GNUC_MINOR__) 
              RW_TEST_GCC_VER
#    endif
#  elif defined (_COMPILER_VERSION) && defined (__sgi)
#    define RW_TEST_COMPILER "SGI MIPSpro, _COMPILER_VERSION = " \
            RW_TEST_STR (_COMPILER_VERSION)
#  elif defined (__INTEL_COMPILER)
#    if defined (_MSC_VER)
#      define RW_TEST_COMPILER "Intel C++, __INTEL_COMPILER = " \
              RW_TEST_STR (__INTEL_COMPILER) ", _MSC_VER = " \
              RW_TEST_STR (_MSC_VER)
#    else
#      define RW_TEST_COMPILER "Intel C++, __INTEL_COMPILER = " \
              RW_TEST_STR (__INTEL_COMPILER)
#    endif
#  elif defined (__EDG__)
#    define RW_TEST_COMPILER "EDG eccp, __EDG_VERSION__ = " \
            RW_TEST_STR (__EDG_VERSION__)
#  elif defined (__HP_aCC)
#    define RW_TEST_COMPILER "HP aCC, __HP_aCC = " \
            RW_TEST_STR (__HP_aCC)
#  elif defined (__IBMCPP__)
#    define RW_TEST_COMPILER "IBM VisualAge C++, __IBMCPP__ = " \
            RW_TEST_STR (__IBMCPP__)
#  elif defined (_MSC_VER)
#    define RW_TEST_COMPILER "MSVC, _MSC_VER = " \
            RW_TEST_STR (_MSC_VER)
#  elif defined (__SUNPRO_CC)
#    define RW_TEST_COMPILER "SunPro, __SUNPRO_CC = " \
            RW_TEST_STR (__SUNPRO_CC)
#  else
#    define RW_TEST_COMPILER "unknown"
#  endif
#endif

#ifndef RW_TEST_LIBSTD
#  ifdef _RWSTD_VER
#    define RW_TEST_LIBSTD "Rogue Wave C++ Standard Library, " \
            "_RWSTD_VER = " RW_TEST_STR (_RWSTD_VER)
#  elif defined (__GLIBCXX__)
#    define RW_TEST_LIBSTD "GNU C++ Standard Library, " \
            "__GLIBCXX__ = " \
            RW_TEST_STR (__GLIBCXX__)
#  elif defined (_STLPORT_VERSION)
     // check for STLport before SGI STL since STLport,
     // being derived from SGI STL, #defines both macros
#    define RW_TEST_LIBSTD "STLport, " \
            "_STLPORT_VERSION = " \
            RW_TEST_STR (_STLPORT_VERSION)
#  elif defined (__SGI_STL)
#    define RW_TEST_LIBSTD "SGI STL, " \
            "__SGI_STL = " \
            RW_TEST_STR (__SGI_STL)
#  elif defined (_YVALS)
     // is there a better way to identify the Dinkumware
     // implementation? does it have a version macro?
#    define RW_TEST_LIBSTD "Dinkum C++ Standard Library"
#  endif
#endif   // RW_TEST_LIBSTD

#ifndef RW_TEST_HARDWARE
#  if defined (__alpha__) || defined (__alpha)
#    define RW_TEST_ARCH "alpha"
#  elif defined (__amd64__) || defined (__amd64)
#    if defined (__LP64__) || defined (_LP64)
#      define RW_TEST_ARCH "amd64/LP64"
#    else
#      define RW_TEST_ARCH "amd64/ILP32"
#    endif
#  elif defined (_PA_RISC2_0)
#    define RW_TEST_ARCH "pa-risc 2.0"
#  elif defined (_PA_RISC1_0)
#    define RW_TEST_ARCH "pa-risc 1.0"
#  elif defined (__hppa)
#    define RW_TEST_ARCH "pa-risc"
#  elif defined (__pentiumpro__) || defined (__pentiumpro)
#    define RW_TEST_ARCH "pentiumpro"
#  elif defined (__pentium__) || defined (__pentium)
#    define RW_TEST_ARCH "pentium"
#  elif defined (__i486__) || defined (__i486)
#    define RW_TEST_ARCH "i486"
#  elif defined (__i386__) || defined (__i386)
#    define RW_TEST_ARCH "i386"
#  elif defined (__i586__) || defined (__i586)
#    define RW_TEST_ARCH "i586"
#  elif defined (__ia64)
#    define RW_TEST_ARCH "ia64"
#  elif defined (__mips)
#    define RW_TEST_ARCH "mips"
#  elif defined (__sparcv9)
#    define RW_TEST_ARCH "sparc-v9"
#  elif defined (__sparcv8)
#    define RW_TEST_ARCH "sparc-v8"
#  elif defined (__sparc)
#    define RW_TEST_ARCH "sparc"
#  elif defined (_POWER)
#    if defined (_ARCH_PWR5)
#      define RW_TEST_ARCH "power-5"
#    elif defined (_ARCH_PWR4)
#      define RW_TEST_ARCH "power-4"
#    elif defined (_ARCH_PWR3)
#      define RW_TEST_ARCH "power-3"
#    elif defined (_ARCH_604)
#      define RW_TEST_ARCH "powerpc-604"
#    elif defined (_ARCH_603)
#      define RW_TEST_ARCH "powerpc-603"
#    elif defined (_ARCH_602)
#      define RW_TEST_ARCH "powerpc-602"
#    elif defined (_ARCH_601)
#      define RW_TEST_ARCH "powerpc-601"
#    elif defined (_ARCH_403)
#      define RW_TEST_ARCH "powerpc-403"
#    elif defined (_ARCH_PPC64)
#      define RW_TEST_ARCH "powerpc/LP64"
#    else
#      define RW_TEST_ARCH "powerpc"
#    endif
#  elif defined (_WIN64)
#    define RW_TEST_ARCH "ia64"
#  elif defined (_WIN32)
#    define RW_TEST_ARCH "i86"
#  elif defined (__x86_64__) || defined (__x86_64)
#    if defined (__LP64__) || defined (_LP64)
#      define RW_TEST_ARCH "x86_64/LP64"
#    else
#      define RW_TEST_ARCH "x86_64/ILP32"
#    endif
#  else
#    define RW_TEST_ARCH "unknown"
#  endif


#  if defined (_AIX54)
#    define RW_TEST_OS "aix-5.4 (or better)"
#  elif defined (_AIX53)
#    define RW_TEST_OS "aix-5.3"
#  elif defined (_AIX52)
#    define RW_TEST_OS "aix-5.2"
#  elif defined (_AIX51)
#    define RW_TEST_OS "aix-5.1"
#  elif defined (_AIX50)
#    define RW_TEST_OS "aix-5.0"
#  elif defined (_AIX43)
#    define RW_TEST_OS "aix-4.3"
#  elif defined (_AIX41)
#    define RW_TEST_OS "aix-4.1"
#  elif defined (_AIX32)
#    define RW_TEST_OS "aix-3.2"
#  elif defined (_AIX)
#    define RW_TEST_OS "aix"
#  elif defined (__hpux)
#    define RW_TEST_OS "hp-ux"
#  elif defined (__osf__)
#    define RW_TEST_OS "tru64-unix"
#  elif defined (__sgi) && defined (__mips)
#    define RW_TEST_OS "irix"
#  elif defined (__linux__) || defined (__linux)

     // get Linux release string (UTS_RELEASE)
#    include <linux/version.h>

#    ifndef UTS_RELEASE
#      define UTS_RELEASE "(unknown release)"
#    endif   // UTS_RELEASE

#    if defined (__ELF__)
#      define LINUX_TYPE "linux-elf"
#    else
#      define LINUX_TYPE "linux"
#    endif

#    define RW_TEST_OS LINUX_TYPE " "     \
            UTS_RELEASE " with glibc "    \
            RW_TEST_STR (__GLIBC__) "."   \
            RW_TEST_STR (__GLIBC_MINOR__)

#  elif defined (__SunOS_5_10)
#    define RW_TEST_OS "sunos-5.10"
#  elif defined (__SunOS_5_9)
#    define RW_TEST_OS "sunos-5.9"
#  elif defined (__SunOS_5_8)
#    define RW_TEST_OS "sunos-5.8"
#  elif defined (__SunOS_5_7)
#    define RW_TEST_OS "sunos-5.7"
#  elif defined (__SunOS_5_6)
#    define RW_TEST_OS "sunos-5.6"
#  elif defined (__sun__)
#    define RW_TEST_OS "sunos"
#  elif defined (_WIN64)
#    define RW_TEST_OS "win64"
#  elif defined (_WIN32)
#    define RW_TEST_OS "win32"
#  else
#    define RW_TEST_OS "unknown"
#  endif

#  define RW_TEST_HARDWARE RW_TEST_ARCH " running " RW_TEST_OS
#else
#  define RW_TEST_HARDWARE "unknown"
#endif

/************************************************************************/

// defined in printf.cpp but not declared in printf.h
_TEST_EXPORT int
rw_vasnprintf (char**, size_t*, const char*, va_list);

/************************************************************************/

enum diag_t {
    diag_trace_0 =  0,   // trace statements of inactive diagnostics
    diag_trace_1 =  1,   // active trace statements (lots of noise)
    diag_debug   =  2,   // debugging statements (less noise)
    diag_info    =  3,   // informational messages (e.g., indicating progress)
    diag_note    =  4,   // noteworthy events
    diag_warn    =  5,   // warnings
    diag_assert  =  6,   // assertions
    diag_error   =  8,   // test logic errors (do not terminate the test)
    diag_fatal   =  9,   // fatal errors (the first will terminate the test)

    N_DIAG_TYPES = diag_fatal + 1,

    diag_default_mask =
      (1 << diag_info)
    | (1 << diag_note)
    | (1 << diag_warn)
    | (1 << diag_assert)
    | (1 << diag_error)
    | (1 << diag_fatal)
};

#define ESC "\x1b"
#define ESC_CODE(fg, bg, attr)   ESC "[" fg bg attr "m"

// ANSI VT100 terminal codes:
#define AT_OFF   "0;"   // all attributes off
#define AT_BLD   "1;"   // bright/bold
#define AT_DIM   "2;"   // dim
// #define AT_???   "3;"   // ???
#define AT_UND   "4;"   // underscore
#define AT_BLI   "5;"   // blink
// #define AT_???   "6;"   // ???
#define AT_RVS   "7;"   // reverse video
#define AT_HID   "8;"   // hidden

#define FG_BLK   "30;"   // foreground black
#define FG_RED   "31;"   // foreground red
#define FG_GRN   "32;"   // foreground green
#define FG_YLW   "33;"   // foreground yellow
#define FG_BLU   "34;"   // foreground blue
#define FG_MAG   "35;"   // foreground magenta
#define FG_CYN   "36;"   // foreground cyan
#define FG_WHT   "37;"   // foreground white

#define BG_BLK   "40;"   // background black
#define BG_RED   "41;"   // background red
#define BG_GRN   "42;"   // background green
#define BG_YLW   "43;"   // background yellow
#define BG_BLU   "44;"   // background blue
#define BG_MAG   "45;"   // background magenta
#define BG_CYN   "46;"   // background cyan
#define BG_WHT   "47;"   // background white


struct esc_text_t {
    char esc  [16];
    char text [16];
};

static esc_text_t diag_msgs[] = {
    { ESC_CODE (FG_CYN, "", AT_DIM),     "ITRACE" },
    { ESC_CODE (FG_BLU, "", ""),         "TRACE" },
    { ESC_CODE (FG_GRN, "", AT_BLI),     "DEBUG" },
    { ESC_CODE ("", "", ""),             "INFO" },
    { ESC_CODE (FG_BLU, BG_YLW, AT_BLI), "NOTE" },
    { ESC_CODE (FG_MAG, "", ""),         "WARNING" },
    { ESC_CODE (FG_RED, "", AT_UND),     "ASSERTION" },
    { ESC_CODE ("", "", ""),             "UNUSED" },   // unused
    { ESC_CODE (FG_WHT, BG_RED, AT_BLD), "ERROR" },
    { ESC_CODE (FG_YLW, BG_RED, AT_BLD), "FATAL" }
};

static const esc_text_t vt100_colors[] = {
    { FG_BLK, "black" },
    { FG_RED, "red" },
    { FG_GRN, "green" },
    { FG_YLW, "yellow" },
    { FG_BLU, "blue" },
    { FG_MAG, "magenta" },
    { FG_CYN, "cyan" },
    { FG_WHT, "white" } 
};

static const esc_text_t vt100_attribs[] = {
    { AT_OFF, "off" },
    { AT_BLD, "bold" },
    { AT_DIM, "dim" },
    { AT_UND, "underscore" },
    { AT_BLI, "blink" },
    { AT_RVS, "reverse" },
    { AT_HID, "hidden" }
};

static char escape_end[] = ESC "[30;30;0m";

// array to store the number of each type of diagnostic
static int ndiags [N_DIAG_TYPES][2] /* = { { failed, attempted }, ... }*/;

static FILE *ftestout;

static jmp_buf test_env;

// diagnostics enabled by defaults
static int trace_mask = diag_default_mask;

// set to 1 after the driver has been initialized
static int driver_initialized = 0;

// set to 1 after the driver has finished running
static int driver_finished = 0;

#if 0   // disabled
// %S: severity
// %M: diagnostic
// %m: diagnostic if not empty
// %F: file name
// %f: file name if not empty
// %C: clause
// %c: clause if not empty
// %L: line number
// %l: line number if valid
// %T: text
// %t: text if not empty
static char diag_pattern [80];
#endif

/// option: use of stdout disabled
static int opt_no_stdout = 0;

// option: verbose diagnostic output
static int opt_verbose = 0;

// option: use CSV format (comma separated values)
static int opt_csv = 0;

// option: use RWTest compatibility format
static int opt_compat = 0;

static char clause_id [80];

/************************************************************************/

#define CHECK_INIT(init, func)   rw_check_init (init, __LINE__, func)

static inline void
rw_check_init (bool init, int line, const char *func)
{
    if (init && !driver_initialized) {
        fprintf (stderr, "%s:%d: %s: test driver already initialized\n",
                 __FILE__, line, func);
        abort ();
    }

    if (!init && driver_initialized) {
        fprintf (stderr, "%s:%d: %s: test driver not initialized yet\n",
                 __FILE__, line, func);
        abort ();
    }

    if (driver_finished) {
        fprintf (stderr, "%s:%d: %s: test finished, cannot call\n",
                 __FILE__, line, func);
    }
}

/************************************************************************/

static int
rw_set_verbose (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Enables verbose mode.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    opt_verbose = 1;
    return 0;
}

/************************************************************************/

static int
rw_set_csv (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Enables CSV (comma separated values) mode.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    opt_csv = 1;
    return 0;
}

/************************************************************************/

static int
rw_set_compat (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Enables RWTest compatiblity mode.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    opt_compat = 1;
    return 0;
}

/************************************************************************/

static int
rw_set_stdout (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Prevents the program from using stdandard output for diagnostic\n"
            "messages. Instead, the driver will create a log file with a name\n"
            "obtained from the from the basename of the program source file,\n"
            "usually obtained by passing the value of the __FILE__ macro to\n"
            "the driver, with the .out extension. If successful, the driver\n"
            "will write all diagnostic messages issued by the program to this\n"
            "file. Otherwise, the driver exits with an error.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }
    
    opt_no_stdout = 1;
    return 0;
}

/************************************************************************/

static int
match_name (const esc_text_t *text, size_t nelems,
            const char *first, const char *last)
{
    assert (0 != text);
    assert (0 != first);
    assert (first <= last);

    const size_t len = size_t (last - first);

    if (0 == len)
        return int (nelems);

    for (size_t i = 0; i != nelems; ++i) {
        if (   0 == strncmp (text [i].text, first, len)
            && '\0' == text [i].text [len])
            return int (i);
    }

    return -1;
}


static int
rw_set_diags (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Sets the colors and names of the diagnostic messages issued by\n"
            "the program.\n"
            "There are 10 different types of diagnostic messages, each with\n"
            "a unique severity level between 0 (the lowest) and 9. Each\n"
            "diagnostic message can either be active or inactive. Each\n"
            "message can have a name of up to 15 characters associated with\n"
            "it, foreground color, background color, and a video attribute.\n"
            "These parameters are controlled by the argument to this option.\n"
            "\nThe syntax of <arg> is as follows:\n"
            "<arg>        ::= <color-list>\n"
            "<color-list> ::= <color-txt> [ ,<color-list> ]\n"
            "<color-text> ::= <sev>:[<color>][:[<color>][:[<attr>][:[<text>]]]]"
            "\n<sev>        ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n"
            "<color>      ::=   black | red | green | yellow\n"
            "                 | blue | magenta | cyan | white\n"
            "<attr>       ::=   off | bold | dim | underscore\n"
            "                 | blink | reverse | hidden\n"
            "<text>       ::= A-Z a-z 0-9 _\n\n"
            "The first <color> component specifies the foreground color\n"
            "while the second (optional) <color> component specifies\n"
            "the background color for the diagnostic of the severity <sev>.\n"
            "Empty arguments are permitted and denote the default system\n"
            "color set for the terminal.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    char *parg = strchr (argv [0], '=');

    if (0 == parg || '\0' == parg [1]) {
        fprintf (stderr, "%s:%d: missing argument in %s\n",
                 __FILE__, __LINE__, argv [0]);
        return 1;
    }

    ++parg;

    // argument syntax:
    //
    // <sev-color-list> := <sev-color-text> [ ,<sev-color-list> ]
    // <sev-color-text> := <sev>:[<color>][:[<color>][:[<attr>][:[<text>]]]]
    // <sev>            := 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
    // <color>          :=   black | red | green | yellow
    //                     | blue | magenta | cyan | white
    // <attr>           :=   off | bold | dim | underscore
    //                     | blink | reverse | hidden
    // <text>           := A-Z a-z 0-9 _


    int ret = 0;

    while (*parg) {

        int severity = -1;
        int fgcol    = -1;
        int bgcol    = -1;
        int attr     = -1;

        char diag_text [sizeof diag_msgs [0].text];
        *diag_text = '\0';

        if ('0' <= *parg && *parg <= '9') {
            severity = *parg++ - '0';
        }
        else {
            // digit expected
            fprintf (stderr, "%s:%d: digit expected at position %d: %s\n",
                 __FILE__, __LINE__, int (parg - argv [0]), argv [0]);

            ret = 1;
            break;
        }

        if (':' == *parg) {
            ++parg;
        }
        else {
            // colon expected
            fprintf (stderr, "%s:%d: colon expected at position %d: %s\n",
                 __FILE__, __LINE__, int (parg - argv [0]), argv [0]);

            ret = 1;
            break;
        }

        char *end = strpbrk (parg, ":,");
        if (0 == end)
            end = parg + strlen (parg);

        fgcol = match_name (vt100_colors, 8, parg, end);
        if (fgcol < 0 || 8 < fgcol) {
            // invalid color
            fprintf (stderr, "%s:%d: unknown color at position %d: %s\n",
                 __FILE__, __LINE__, int (parg - argv [0]), argv [0]);

            ret = 1;
            break;
        }

        if (':' == *end || '\0' == *end) {
            parg = end + (0 != *end);
            end  = strpbrk (parg, ":,");
            if (0 == end)
                end = parg + strlen (parg);

            bgcol = match_name (vt100_colors, 8, parg, end);

            if (bgcol < 0 || 8 < bgcol) {
                // invalid color
                fprintf (stderr, "%s:%d: unknown color at position %d: %s\n",
                         __FILE__, __LINE__, int (parg - argv [0]), argv [0]);

                ret = 1;
                break;
            }

            if (':' == *end || '\0' == *end) {
                parg = end + ('\0' != *end);
                end  = strpbrk (parg, ":,");
                if (0 == end)
                    end = parg + strlen (parg);

                attr = match_name (vt100_attribs, 8, parg, end);
                if (attr < 0 || 8 < attr) {
                    // invalid attribute
                    fprintf (stderr,
                             "%s:%d: unknown attribute at position %d: %s\n",
                             __FILE__, __LINE__, int (parg - argv [0]),
                             argv [0]);

                    ret = 1;
                    break;
                }

                if (':' == *end || '\0' == *end) {
                    parg = end + (0 != *end);
                    end  = strpbrk (parg, ":,");
                    if (0 == end)
                        end = parg + strlen (parg);

                    size_t len = size_t (end - parg);

                    if (sizeof diag_msgs [severity].text < len) {
                        
                        // name too long
                        fprintf (stderr,
                                 "%s:%d: name too long at position %d: %s\n",
                                 __FILE__, __LINE__, int (parg - argv [0]),
                                 argv [0]);

                        len = sizeof diag_msgs [severity].text;
                    }

                    memcpy (diag_text, parg, len);
                    diag_text [len] = '\0';
                }
            }
        }

        strcpy (diag_msgs [severity].esc, ESC "[");

        if (fgcol < 8)
            strcat (diag_msgs [severity].esc, vt100_colors [fgcol].esc);

        if (bgcol < 8) {
            strcat (diag_msgs [severity].esc, vt100_colors [bgcol].esc);

            const size_t bgdiginx = strlen (diag_msgs [severity].esc) - 3;

            assert ('3' == diag_msgs [severity].esc [bgdiginx]);
            diag_msgs [severity].esc [bgdiginx] = '4';
        }

        if (attr < 8)
            strcat (diag_msgs [severity].esc, vt100_attribs [attr].esc);

        if (diag_msgs [severity].esc [2])
            strcat (diag_msgs [severity].esc, "m");
        else
            diag_msgs [severity].esc [0] = '\0';

        if (*diag_text)
            strcpy (diag_msgs [severity].text, diag_text);

        parg = end + ('\0' != *end);
    }

    return ret;
}

/************************************************************************/

static int
rw_set_trace_mask (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Specifies the severity of diagnostic messages to be issued when\n"
            "active. By default, diagnostics with severity of 3 and and above\n"
            "are issued, all others are suppressed. The severity of an inactive"
            "\ndiagnostic is always zero, regardless of what the severity would"
            "\nbe if it were active.\n\n"
            "The syntax of <arg> is as follows:\n"
            "<arg>   ::= <range> [ , <range> ]\n"
            "<range> ::= [ - ] <digit> | <digit> - [ <digit> ]\n"
            "<digit> ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9\n"
            "\n"
            "The default names of the diagnostic messages arranged in order\n"
            "of increasing severity are as follows:\n"
            "ITRACE, TRACE, DEBUG, INFO, NOTE, WARNING, ASSERTION, UNUSED,\n"
            "ERROR, and FATAL.\n"
            "The name of each inactive diagnostic is preceded by the name\n"
            "of the severity-0 diagnostic (ITRACE_ by default). The severity\n"
            "of an inactive diagnostic is always zero, regardless of what\n"
            "the severity would be if it were active.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    char *parg = strchr (argv [0], '=');

    if (0 == parg) {
        trace_mask = -1;
        return 0;
    }
        
    int new_mask = 0;

    const char* const argbeg = ++parg;

    // the lower bound of a range of severities to be enabled or disabled
    // negative values are not valid and denote an implicit lower bound
    // of 1 (such as in "-3" which is a shorthand for "1-3")
    long first = -1;

    for ( ; '\0' != *parg ; ) {

        // skip any leading whitespace
        for ( ; ' ' == *parg; ++parg);

        if ('-' == *parg) {
            if (first < 0) {
                first = 0;
                ++parg;
            }
            else {
                fprintf (stderr,
                         "invalid character '%c' at position %d: \"%s\"\n",
                         *parg, int (parg - argbeg), argv [0]);
                return 2;
            }
        }

        // parse a numeric argument
        char *end;
        long severity = strtol (parg, &end, 0);

        // skip any trailing whitespace
        for ( ; ' ' == *end; ++end);

        if (end == parg || '-' != *end && ',' != *end && '\0' != *end) {
            fprintf (stderr,
                     "invalid character '%c' at position %d: \"%s\"\n",
                     *end, int (parg - argbeg), argv [0]);
            return 2;
        }

        if (0 <= first) {
            if (severity < 0 || N_DIAG_TYPES < severity) {
                fprintf (stderr,
                         "invalid value %ld at position %d: \"%s\"\n",
                         severity, int (parg - argbeg), argv [0]);
                return 2;
            }

            ++severity;

            if ((',' == *end || '-' == *end) && end [1])
                ++end;
        }
        else if (',' == *end) {
            first = severity++;
            if ('\0' == end [1]) {
                fprintf (stderr,
                         "invalid character '%c' at position %d: \"%s\"\n",
                         *end, int (parg - argbeg), argv [0]);
                return 2;
            }

            ++end;
        }
        else if ('-' == *end) {
            first = severity;
            while (' ' == *++end);
            if ('\0' == *end) {
                severity = N_DIAG_TYPES;
            }
            else if  (',' == *end) {
                severity = N_DIAG_TYPES;
                ++end;
            }
            else
                severity = -1;
        }
        else if ('\0' == *end) {
            first = severity++;
        }
        else {
            fprintf (stderr,
                     "invalid character '%c' at position %d: \"%s\"\n",
                     *end, int (parg - argbeg), argv [0]);
            return 2;
        }

        parg = end;

        if (0 <= first && first < severity) {
            for (int i = first; i != severity; ++i)
                new_mask |= 1 << i;
            severity = -1;
        }
    }

    trace_mask = new_mask;

    return 0;
}

/************************************************************************/

static int
rw_set_trace (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Specifies that diagnostic messages of all severities be issued\n"
            "regardless of whether they are active or not. See the --severity\n"
            "option for details.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    return rw_set_trace_mask (argc, argv);
}

/************************************************************************/

static int
rw_set_output_file (int argc, char *argv[])
{
    if (1 == argc && argv && 0 == argv [0]) {
        static const char helpstr[] = {
            "Specifies the name of the output file to be used by the program\n"
            "for diagnostic messages. Unless this option is specified, the\n"
            "program will issue all diagnostic messages to the standard output."
            "\nDriver diagnostics are always directed to stderr.\n"
        };

        argv [0] = _RWSTD_CONST_CAST (char*, helpstr);

        return 0;
    }

    const char *file_name = 0;

    if ('-' == argv [0][0] && 'O' == argv [0][1] || 'o' == argv [0][1]) {
        file_name = argv [0] + 2;
    }
    else if (1 < argc && '-' != argv [1][0]) {
        file_name = argv [1];
    }

    if (file_name) {

        FILE* const f = fopen (file_name, "w");

        if (f) {
            if (ftestout && ftestout != stderr)
                fclose (ftestout);

            ftestout = f;
        }
    }

    // return 0 on success, any non-zero value on failure
    return !(ftestout != 0);
}

/************************************************************************/

_TEST_EXPORT int
rw_vsetopts (const char *opts, va_list va);

_TEST_EXPORT int
rw_vtest (int argc, char **argv,
          const char *file_name,
          const char *clause,
          const char *comment,
          int (*fun)(int, char**),
          const char *optstr,
          va_list     va)
{
    CHECK_INIT (false, "rw_vtest()");

    driver_initialized = 1;

    if (optstr && 0 > rw_vsetopts (optstr, va)) {
        fprintf (stderr, "%s:%d: rw_setopts() failed\n", __FILE__, __LINE__);
        return 1;
    }

    if (3 > rw_setopts ("|-no-stdout "
                        "|-diags= "      // argument required
                        "|-trace "
                        "|-severity= "   // argument required
                        "|-csv "
                        "|-compat "
                        "o|-output:"     // argument optional
                        "v|-verbose",
                        rw_set_stdout,
                        rw_set_diags,
                        rw_set_trace,
                        rw_set_trace_mask,
                        rw_set_csv,
                        rw_set_compat,
                        rw_set_output_file,
                        rw_set_verbose,
                        0)) {
        fprintf (stderr, "%s:%d: rw_setopts() failed\n", __FILE__, __LINE__);
        abort ();
        return 1;
    }

#ifndef _RWSTD_USE_CONFIG

    // enable RWTest compatibility mode
    rw_set_compat (0, 0);

    // disable output to stdout
    rw_set_stdout (0, 0);

#endif   // _RWSTD_USE_CONFIG

    int status = rw_runopts (argc, argv);

    if (status)
        return status;

    if (0 == ftestout) {

        if (opt_no_stdout && file_name) {
            char fname [256];

            const char* const slash = strrchr (file_name, _RWSTD_PATH_SEP);
            strcpy (fname, slash ? slash + 1 : file_name);

            char* const dot = strchr (fname, '.');
            if (dot)
                strcpy (dot, ".out");
            else
                strcat (fname, ".out");

            ftestout = fopen (fname, "w");
        }
        else
            ftestout = stdout;
    }

    if (clause)
        strcpy (clause_id, clause);

    const char begin_fmt[] = {
        "\n"
        "# COMPILER: %s\n"
        "# ENVIRONMENT: %s\n"
        "# FILE: %s\n"
        "# COMPILED: %s, %s\n"
        "# COMMENT: %s\n"
        "######################################################"
    };

    const char* const fname = strrchr (file_name, _RWSTD_PATH_SEP);

    rw_info (0, 0, 0,
             begin_fmt,
             RW_TEST_COMPILER, RW_TEST_HARDWARE,
             fname ? fname + 1 : file_name,
             __DATE__, __TIME__,
             comment ? comment : "");

    status = setjmp (test_env);

    if (0 == status) {
        // environment set, execute the callback function
        status = fun (argc, argv);
    }
    else {
        // fatal test error (via a call to rw_fatal())
    }

    driver_finished = 1;

    fprintf (ftestout,
             "# +-----------------------+--------+--------+--------+\n"
             "# | DIAGNOSTIC            | ACTIVE |  TOTAL |   PASS |\n"
             "# +-----------------------+--------+--------+--------+\n");

    int nlines = 0;

    for (int i = 0; i != N_DIAG_TYPES; ++i) {
        if (ndiags [i][0] || trace_mask & (diag_debug | diag_trace_1)) {

            // print out details for any non-zero totals
            // or for all totals when debugging or tracing
            // is enabled

            ++nlines;

            const long num = (ndiags [i][0] - ndiags [i][1]) * 100L;
            const long den = ndiags [i][0];

            const long pct = den ? num / den : 0;

            fprintf (ftestout,
                     "# | (S%d) %-*s | %6d | %6d | %5ld%% |\n",
                     i, sizeof diag_msgs [i].text, diag_msgs [i].text,
                     ndiags [i][1], ndiags [i][0], pct);
        }
    }

    if (0 == nlines)
        fprintf (ftestout, "# no diagnostics\n");

    fprintf (ftestout,
             "# +-----------------------+--------+--------+--------+\n");

    if (opt_compat) {

        // TO DO: get rid of this

        // RWTest compatibility format

        fprintf (ftestout,
                 "######################################################\n"
                 "## Warnings = %d\n"
                 "## Assertions = %d\n"
                 "## FailedAssertions = %d\n",
                 ndiags [diag_warn][1],
                 ndiags [diag_assert][0],
                 ndiags [diag_assert][1]);
    }

    fclose (ftestout);
    ftestout = 0;

    return status;
}

/************************************************************************/

_TEST_EXPORT int
rw_test (int argc, char **argv,
         const char *fname,
         const char *clause,
         const char *comment,
         int (*testfun)(int, char**),
         const char *optstr,
         ...)
{
    CHECK_INIT (false, "rw_test()");

    va_list va;
    va_start (va, optstr);

    const int status =
        rw_vtest (argc, argv, fname, clause, comment, testfun, optstr, va);

    va_end (va);

    return status;
}

/************************************************************************/

// escape every occurrence of the double quote character in the string
// pointed to by buf by prepending to it the escape character specified
// by the last acrgument
// returns the new buffer if the size of the existing buffer isn't
// sufficient and sets *pbufsize to the size of the newly allocated
// buffer, otherwise the original value of buf and leaves *pbufsize
// unchanged
static char*
rw_escape (char *buf, size_t bufsize, char esc)
{
    // handle null buffer
    if (0 == buf)
        return buf;

    // count the number of embedded quotes
    char *quote = buf;
    size_t nquotes = 0;
    while ((quote = strchr (quote, '"'))) {
        ++nquotes;
        ++quote;
    }

    // no quotes found, return the original buffer
    if (0 == nquotes)
        return buf;

    // conpute the size of the buffer that will be needed to escape
    // all the double quotes
    size_t newbufsize = strlen (buf) + nquotes + 1;

    char *newbuf = 0;

    if (0 /* newbufsize <= bufsize */) {
        // FIXME: escape embedded quotes in place w/o reallocation
        _RWSTD_UNUSED (bufsize);
    }
    else {
        newbuf = (char*)malloc (newbufsize);
        if (0 == newbuf) {
            return 0;
        }

        // set the next pointer to the beginning of the new buffer
        // as the destination where to copy the string argument
        char *next = newbuf;

        // set quote to initially point to the beginning of
        // the source buffer and then just past the last quote
        quote = buf;

        for (char *q = buf; ; ++q) {

            // look for the next (or first) quote
            q = strchr (q, '"');

            // compute the number of characters, excluding the quote
            // to copy to the destination buffer
            const size_t nchars = q ? size_t (q - quote) : strlen (quote);

            memcpy (next, quote, nchars);

            if (q) {
                // append the escape character to the destination buffer
                next [nchars] = esc;

                // append the quote from the source string
                next [nchars + 1] = '"';

                // advance the destination pointer past the quote
                next += nchars + 2;

                // advance the source pointer past the embedded quote
                quote = q + 1;
            }
            else {
                // NUL-terminate the destination buffer
                *next = '\0';
                break;
            }
        }
    }

    return newbuf;
}

/************************************************************************/

static void
rw_vdiag (diag_t diag, int severity, const char *file, int line,
          const char *fmt, va_list va)
{
    CHECK_INIT (true, "rw_vdiag()");

    // normalize the severity
    if (diag)
        severity = diag * severity;

    if (severity < 0)
        severity = 0;
    else if (N_DIAG_TYPES <= severity)
        severity = N_DIAG_TYPES - 1;

    // increment the appropriate diagnostic counter
    ++ndiags [diag][0];

    if (severity)
        ++ndiags [diag][1];

    if ((1 << severity) & trace_mask) {

        assert (0 != fmt);

        char *usrbuf = 0;
        const int nchars = rw_vasnprintf (&usrbuf, 0, fmt, va);

        if (nchars < 0)
            usrbuf = _RWSTD_CONST_CAST (char*, "*error formatting string*");

        // compute the number of newline characters in the text
        int nlines = 0;
        for (const char *nl = usrbuf; (nl = strchr (nl, '\n')); ++nl)
            ++nlines;

#ifndef _RWSTD_NO_ISATTY

        // is output sent to a terminal?
        // if so, assume a vt100 compatible terminal for now
        static const int tty = isatty (fileno (ftestout));

#else   // if defined (_RWSTD_NO_ISATTY)

        // FIXME: deal with a missing isatty() and Windows
        static const int tty = 0;

#endif   // _RWSTD_NO_ISATTY

        const char* const diagstr[] = {
            tty ? diag_msgs [severity].esc : "",
            *diag_msgs [severity].text ? diag_msgs [severity].text : "UNKNOWN",
            tty && *diag_msgs [severity].esc ? escape_end : ""
        };

        const char* const traced_diag =
            0 == severity && diag_msgs [diag].text ? diag_msgs [diag].text : 0;

        const char* const slash = file ? strrchr (file, _RWSTD_PATH_SEP) : 0;
        if (slash)
            file = slash + 1;

        char *mybuf = 0;

        if (opt_csv) {

            // format all fields as comma separated values (CSV):
            // -- a field containing the quote character, the comma,
            //    or the newline or linefeed character must be enclosed
            //    in a pair of double quotes
            // -- every occurrence of the double quote character in a field
            //    must be escaped by prepening another double quote character
            //    to it

            // escape all double quotes by prepending the double
            // quote character to each according to the CSV format
            char* const newbuf = rw_escape (usrbuf, 0, '"');
            if (newbuf != usrbuf) {
                free (usrbuf);
                usrbuf = newbuf;
            }

            mybuf =
                rw_sprintfa ("%d, "                      // severity
                             "\"%s%s"                    // diagnostic
                             "%{?}_%s%{;}%s\", "         // traced diagnostic
                             "\"%s\", "                  // clause
                             "\"%s\", "                  // file
                             "%d, "                      // line
                             "\"%s\"",                   // user text
                             severity,
                             diagstr [0], diagstr [1],
                             0 != traced_diag, traced_diag, diagstr [2],
                             clause_id,
                             0 != file ? file : "",
                             line,
                             usrbuf);
        }
        else if (opt_verbose) {

            nlines += 2 + ('\0' != *clause_id) + (0 != file) + (0 < line);

            mybuf =
                rw_sprintfa ("# %s%s"                    // diagnostic
                             "%{?}_%s%{;}%s "            // traced diagnostic
                             "(S%d) "                    // severity
                             "(%d lines):\n"             // number of lines
                             "# TEXT: %s\n"              // user text
                             "%{?}# CLAUSE: %s\n%{;}"    // clause if not empty
                             "%{?}# FILE: %s\n%{;}"      // file if not null
                             "%{?}# LINE: %d\n%{;}",     // line if positive
                             diagstr [0], diagstr [1],
                             0 != traced_diag, traced_diag, diagstr [2],
                             severity,
                             nlines,
                             usrbuf,
                             '\0' != *clause_id, clause_id,
                             0 != file, file,
                             0 < line, line);
        }
        else {

            mybuf =
                rw_sprintfa ("# %s%s"                 // diagnostic
                             "%{?}_%s%{;}%s "         // traced diagnostic
                             "(S%d): "                // severity
                             "%{?}[%s] %{;}"          // clause if not empty
                             "%{?}(%d lines): %{;}"   // number of lines if > 1
                             "%{?}%s:"                // if (file) then file
                             "%{?}%d:%{;} "           //   if (0 < line) line
                             "%{:}"                   // else
                             "%{?}line %d: %{;}"      //   if (0 < line) line
                             "%{;}"                   // endif
                             "%s",                    // user text
                             diagstr [0], diagstr [1],
                             0 != traced_diag, traced_diag, diagstr [2],
                             severity,
                             '\0' != *clause_id, clause_id,
                             1 < nlines, nlines,
                             0 != file, file,
                             0 < line, line,
                             0 < line, line,
                             usrbuf);
        }

        fprintf (ftestout, "%s\n", mybuf);

        free (mybuf);
        free (usrbuf);
    }

    if (diag_fatal == diag && severity) {
        // fatal error, terminate test
        longjmp (test_env, severity);
    }
}

/************************************************************************/

_TEST_EXPORT int
rw_fatal (int success, const char *file, int line, const char *fmt, ...)
{
    CHECK_INIT (true, "rw_fatal()");

    va_list va;
    va_start (va, fmt);

    rw_vdiag (diag_fatal, 0 == success, file, line, fmt, va);

    va_end (va);

    return success;
}

/************************************************************************/

_TEST_EXPORT int
rw_error (int success, const char *file, int line, const char *fmt, ...)
{
    CHECK_INIT (true, "rw_error()");

    va_list va;
    va_start (va, fmt);

    rw_vdiag (diag_error, 0 == success, file, line, fmt, va);

    va_end (va);

    return success;
}

/************************************************************************/

_TEST_EXPORT int
rw_assert (int success, const char *file, int line, const char *fmt, ...)
{
    CHECK_INIT (true, "rw_assert()");

    va_list va;
    va_start (va, fmt);

    rw_vdiag (diag_assert, 0 == success, file, line, fmt, va);

    va_end (va);

    return success;
}

/************************************************************************/

_TEST_EXPORT int
rw_warn (int success, const char *file, int line, const char *fmt, ...)
{
    CHECK_INIT (true, "rw_warn()");

    va_list va;
    va_start (va, fmt);

    rw_vdiag (diag_warn, 0 == success, file, line, fmt, va);

    va_end (va);

    return success;
}

/************************************************************************/

_TEST_EXPORT int
rw_note (int success, const char *file, int line, const char *fmt, ...)
{
    CHECK_INIT (true, "rw_note()");

    va_list va;
    va_start (va, fmt);

    rw_vdiag (diag_note, 0 == success, file, line, fmt, va);

    va_end (va);

    return success;
}

/************************************************************************/

_TEST_EXPORT int
rw_info (int success, const char *file, int line, const char *fmt, ...)
{
    CHECK_INIT (true, "rw_info()");

    va_list va;
    va_start (va, fmt);

    rw_vdiag (diag_info, 0 == success, file, line, fmt, va);

    va_end (va);

    return success;
}

/************************************************************************/

_TEST_EXPORT int
rw_debug (int success, const char *file, int line, const char *fmt, ...)
{
    CHECK_INIT (true, "rw_debug()");

    va_list va;
    va_start (va, fmt);

    rw_vdiag (diag_debug, 0 == success, file, line, fmt, va);

    va_end (va);

    return success;
}

/************************************************************************/

_TEST_EXPORT int
rw_trace (int success, const char *file, int line, const char *fmt, ...)
{
    CHECK_INIT (true, "rw_trace()");

    va_list va;
    va_start (va, fmt);

    rw_vdiag (diag_trace_1, 0 == success, file, line, fmt, va);

    va_end (va);

    return success;
}
