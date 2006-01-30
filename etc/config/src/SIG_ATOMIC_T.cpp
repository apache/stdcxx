// checking for signal support in <signal.h>

#include <signal.h>
#include <stdio.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

// establish a dependency on the test for long long
// and #define the LONG_LONG macro used in "type.h"
#ifndef _RWSTD_NO_LONG_LONG
#  define LONG_LONG long long
#else   // if defined (_RWSTD_NO_LONG_LONG)
#  if defined (_MSC_VER)
#    define LONG_LONG   __int64
#  endif   // _MSC_VER
#endif   // _RWSTD_NO_LONG_LONG

#include "types.h"   // for type_name()


int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_SIG_ATOMIC_T\n");

#endif   // _RWSTD_USE_CONFIG

#ifdef SIG_DFL
    printf ("#define _RWSTD_SIG_DFL         %2d\n", SIG_DFL);
#endif   // SIG_DFL

#if defined (SIG_ERR)
    printf ("#define _RWSTD_SIG_ERR         %2d\n", SIG_ERR);
#endif   // SIG_ERR

#if defined (SIG_IGN)
    printf ("#define _RWSTD_SIG_IGN         %2d\n", SIG_IGN);
#endif   // SIG_IGN

#if defined (SIGABRT)
    printf ("#define _RWSTD_SIGABRT         %2d\n", SIGABRT);
#endif   // SIGABRT

#if defined (SIGFPE)
    printf ("#define _RWSTD_SIGFPE          %2d\n", SIGFPE);
#endif   // SIGFPE

#if defined (SIGILL)
    printf ("#define _RWSTD_SIGILL          %2d\n", SIGILL);
#endif   // SIGILL

#if defined (SIGINT)
    printf ("#define _RWSTD_SIGINT          %2d\n", SIGINT);
#endif   // SIGINT

#if defined (SIGSEGV)
    printf ("#define _RWSTD_SIGSEGV         %2d\n", SIGSEGV);
#endif   // SIGSEGV

#if defined (SIGTERM)
    printf ("#define _RWSTD_SIGTERM         %2d\n", SIGTERM);
#endif   // SIGTERM

    //////////////////////////////////////////////////////////////////
    // determine the underlying arithmetic type

    sig_atomic_t atomic = 0;
    const char* const tname = type_name (atomic);
    printf ("#define _RWSTD_SIG_ATOMIC_T   %s\n", tname);

    // compute the type's minimum and maximum
    const char* const symbol = type_name (++atomic);

    printf ("#define _RWSTD_SIG_ATOMIC_MIN _RWSTD_%s_MIN\n"
            "#define _RWSTD_SIG_ATOMIC_MAX _RWSTD_%s_MAX\n",
            symbol, symbol);

    return 0;
}
