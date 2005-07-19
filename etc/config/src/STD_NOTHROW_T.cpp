// checking for std::nothrow_t

#include <stddef.h>   // for size_t

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG


// will fail if namespaces aren't supported
namespace std
{
    struct nothrow_t { };
}


// force a failure if namespace std isn't honored
void nothrow_t ();


#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define throw()
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION


// try to use ::operator new (size_t, std::nothrow_t)
// and guess whether std::nothrow_t is defined or not
void* operator new (size_t, const std::nothrow_t&) throw ();


int main ()
{
    ::operator new (1, std::nothrow_t ());

    return 0;
}
