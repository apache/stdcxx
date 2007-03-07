// checking for ::nothrow_t

#include <stddef.h>   // for size_t

#include "config.h"


// declare a global nothrow_t
struct nothrow_t { };


#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define throw()
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

void* operator new (size_t n, const nothrow_t&) throw ();

int main ()
{
    // won't link if ::operator new (size_t, const ::nothrow_t&)
    // is not defined in the compiler support library
    ::operator new (1, nothrow_t ());

    return 0;
}
