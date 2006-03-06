// checking for atomic operations

#if defined (_WIN32) || defined (_WIN64)

#include <stdio.h>
#include <windows.h>


extern "C" {

typedef int  __stdcall pfiipv_t (int*);
typedef int  __stdcall pfiip_t (volatile int*);
typedef long __stdcall pfllp_t (long*);
typedef long __stdcall pfllpv_t (volatile long*);

}   // extern "C"


const char* foo (pfiip_t) { return "int"; }
const char* foo (pfiipv_t) { return "volatile int"; }

const char* foo (pfllp_t) { return "long"; }
const char* foo (pfllpv_t) { return "volatile long"; }


int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_ATOMIC_OPS\n");

#endif   // _RWSTD_USE_CONFIG

    // determine the argument type of InterlockedIncrement()
    // (the type changes from long* to volatile long* depending
    // on the version/patch of MSVC)

    printf ("#define _RWSTD_INTERLOCKED_T %s\n", foo (InterlockedIncrement));

    return 0;
}

#else   // not Windows

#include <stdio.h>

int main ()
{
#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_ATOMIC_OPS\n");

#endif   // _RWSTD_USE_CONFIG

    return 0;
}

#endif   // Windows
