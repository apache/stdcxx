// checking for signal support in <signal.h>

#include <signal.h>
#include <stdio.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


const char* foo (char)               { return "char"; }
const char* foo (short)              { return "short"; }
const char* foo (unsigned short)     { return "unsigned short"; }
const char* foo (int)                { return "int"; }
const char* foo (unsigned int)       { return "unsigned int"; }
const char* foo (long)               { return "long"; }
const char* foo (unsigned long)      { return "unsigned long"; }

#ifndef _RWSTD_NO_LONG_LONG

const char* foo (long long)          { return "long long"; }
const char* foo (unsigned long long) { return "unsigned long long"; }

#endif   // _RWSTD_NO_LONG_LONG


int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_SIG_ATOMIC_T\n");

#endif   // _RWSTD_USE_CONFIG

#ifdef SIG_DFL
    printf ("#define _RWSTD_SIG_DFL %d\n", SIG_DFL);
#endif   // SIG_DFL

#if defined (SIG_ERR)
    printf ("#define _RWSTD_SIG_ERR %d\n", SIG_ERR);
#endif   // SIG_ERR

#if defined (SIG_IGN)
    printf ("#define _RWSTD_SIG_IGN %d\n", SIG_IGN);
#endif   // SIG_IGN

#if defined (SIGABRT)
    printf ("#define _RWSTD_SIGABRT %d\n", SIGABRT);
#endif   // SIGABRT

#if defined (SIGFPE)
    printf ("#define _RWSTD_SIGFPE %d\n", SIGFPE);
#endif   // SIGFPE

#if defined (SIGILL)
    printf ("#define _RWSTD_SIGILL %d\n", SIGILL);
#endif   // SIGILL

#if defined (SIGINT)
    printf ("#define _RWSTD_SIGINT %d\n", SIGINT);
#endif   // SIGINT

#if defined (SIGSEGV)
    printf ("#define _RWSTD_SIGSEGV %d\n", SIGSEGV);
#endif   // SIGSEGV

#if defined (SIGTERM)
    printf ("#define _RWSTD_SIGTERM %d\n", SIGTERM);
#endif   // SIGTERM

    sig_atomic_t atomic = 0;
    printf ("#define _RWSTD_SIG_ATOMIC_T %s\n", foo (atomic));

    return 0;
}
