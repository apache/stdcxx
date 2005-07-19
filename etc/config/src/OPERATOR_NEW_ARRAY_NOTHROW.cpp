// checking for operator new[] (size_t, nothrow_t)

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#include <stddef.h>


#ifndef _RWSTD_NO_NAMESPACE
# ifndef _RWSTD_NO_STD_NOTHROW_T   

namespace std {

# endif // _RWSTD_NO_STD_NOTHROW_T
#endif // _RWSTD_NO_NAMESPACE
    
struct nothrow_t { };

#ifndef _RWSTD_NO_NAMESPACE
# ifndef _RWSTD_NO_STD_NOTHROW_T   

}  // end namespace std

using std::nothrow_t;

# endif // _RWSTD_NO_STD_NOTHROW_T
#endif // _RWSTD_NO_NAMESPACE

#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define throw()
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

void* operator new[] (size_t, const nothrow_t&) throw ();

int main ()
{
    ::operator new[] (1, nothrow_t ());
    return 0;
}
