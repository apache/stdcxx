// checking the contents of <unistd.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

#if (defined (_WIN32) || defined (_WIN64)) && !defined (__CYGWIN__)
#  include <io.h>
#else
#  include <unistd.h>
#endif   // _WIN32 || _WIN64

#ifndef STDIN_FILENO
#  define STDIN_FILENO    0
#  define STDOUT_FILENO   1
#  define STDERR_FILENO   2
#endif   // STDIN_FILENO

#if defined (_RWSTD_NO_SSIZE_T)
#  define ssize_t long
#endif

static void print_macros ()
{
    struct {
        int value;
        const char *name;
    } macros[] = {

#define MACRO(name)   { name, #name }
        MACRO (STDIN_FILENO),
        MACRO (STDOUT_FILENO),
        MACRO (STDERR_FILENO),

        // open(2) oflag constants
        MACRO (O_RDONLY),
        MACRO (O_WRONLY),
        MACRO (O_RDWR),

#ifdef O_ACCMODE
        // fcntl(2) file access mask
        MACRO (O_ACCMODE),
#endif  // O_ACCMODE

        MACRO (O_APPEND),
        MACRO (O_EXCL),
        MACRO (O_CREAT),
        MACRO (O_TRUNC),

        // lseek(2) whence constants
        MACRO (SEEK_SET),
        MACRO (SEEK_CUR),
        MACRO (SEEK_END)

#ifdef F_GETFL
        // fcntl(2) cmd argument
        , MACRO (F_GETFL)
#endif
    };

    for (unsigned i = 0; i != sizeof macros / sizeof *macros; ++i)
        printf ("#define _RWSTD_%-20s %#8x   // %s\n",
                macros [i].name, macros [i].value, macros [i].name);
}

// POSIX: off_t is a signed integral type
const char* off_t_name (short)                { return "short"; }
const char* off_t_name (int)                  { return "int"; }
const char* off_t_name (long)                 { return "long"; }

// POSIX: ssize_t is a signed integral type
const char* ssize_t_name (short)              { return "short"; }
const char* ssize_t_name (unsigned short)     { return "unsigned short"; }
const char* ssize_t_name (int)                { return "int"; }
const char* ssize_t_name (unsigned int)       { return "unsigned int"; }
const char* ssize_t_name (long)               { return "long"; }
const char* ssize_t_name (unsigned long)      { return "unsigned long"; }

#ifndef _RWSTD_NO_LONG_LONG

const char* off_t_name (long long)            { return "long long"; }

const char* ssize_t_name (long long)          { return "long long"; }
const char* ssize_t_name (unsigned long long) { return "unsigned long long"; }

#endif   // _RWSTD_NO_LONG_LONG


int main ()
{
#ifndef _RWSTD_USE_CONFIG

    printf ("/**/\n#undef _RWSTD_UNISTD_DECL\n");

#endif   // _RWSTD_USE_CONFIG

    print_macros ();

    off_t off = 0;
    printf ("#define _RWSTD_OFF_T %s   // off_t\n", off_t_name (off));

    ssize_t s = 0;
    printf ("#define _RWSTD_SSIZE_T %s   // ssize_t\n", ssize_t_name (s));

    return 0;
}
