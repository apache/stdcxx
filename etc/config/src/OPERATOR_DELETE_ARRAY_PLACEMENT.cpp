// checking for operator delete[] (void*, void*)

#include "config.h"

#include <stddef.h>

#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define throw()
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

loop:

    while ((ip [0] = ip [1])) {  // force a SIGSEGV
        ++ip;
        terminate ();            // recurse infinitely
    }

    goto loop;
}

}   // namespace std

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


void operator delete[] (void*, void*) throw ();

void foo (void *p0, void *p1) throw () { ::operator delete[] (p0, p1); }

void (*op_delete_array)(void*, void*);


int dtor_calls;

struct S {
    int dummy_;

    ~S () { ++dtor_calls; }
};


int main (int argc, char *argv[])
{
    (void)&argv;

    int result = 0;

    char buf [sizeof (S) + 32];

    void *p0 = buf;

    ::operator delete[] (p0, p0);

    if (0 != dtor_calls)
        result = (result << 1) + 1;

    if (argc > 1)
        op_delete_array = &operator delete[];
    else
        op_delete_array = &foo;

    op_delete_array (p0, p0);

    if (0 != dtor_calls)
        result = (result << 1) + 1;

    // prevent compilers from optimizing the code above away
    // without actually ever using the result (unless the
    // test is invoked with command line arguments)
    if (argc > 1)
        return result;

    return 0;
}
