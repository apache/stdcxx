// checking for operator delete (void*, nothrow_t)

#include "config.h"
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


void operator delete (void*, const nothrow_t&) throw ();

int main ()
{
    void* ptr = 0;
    ::operator delete(ptr, nothrow_t ());
    return 0;
}
