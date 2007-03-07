// checking for operator new[] (size_t, void*)

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


void* operator new[] (size_t, void*) throw ();

void* foo (size_t n, void *p) throw () { return ::operator new[] (n, p); }

void* (*operator_new_array)(size_t, void*);


struct S {
    char c_;
    char d_;

    S (): c_ ('c'), d_ ('d') { }
};


int main (int argc, char *argv[])
{
    (void)&argv;

    int result = 0;

    char buf [sizeof (S) + 32];

    void *p0 = buf;

    void *p1 = ::operator new[] (sizeof (S), p0);

    if (p1 != p0)
        result = result << 1 + 1;

    if (argc > 1)
        operator_new_array = &operator new[];
    else
        operator_new_array = &foo;
        
    p1 = operator_new_array (sizeof (S), p0);

    if (p1 != p0)
        result = (result << 1) + 1;

    S *s = new (p0) S [1];

    if (p1 != p0)
        result = (result << 1) + 1;

    if (s->c_ != 'c' || s->d_ != 'd')
        result = (result << 1) + 1;

    // prevent compilers from optimizing the code above away
    // without actually ever using the result (unless the
    // test is invoked with command line arguments)
    if (argc > 1)
        return result;

    return 0;
}
