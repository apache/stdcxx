// checking for operator new (size_t, nothrow_t)

/*********************************************************************
 * This and related operator comptests are necessarily complicated
 * due to the possibility that the nothrow versions of the operators
 * may be defined in the compiler's language support library in the
 * global namespace rather than in namespace std.
 *
 * STD_NOTHROW_T.cpp, checks if operator new(size_t, std::nothrow_t)
 * is defined in the language support library. If it isn't,
 * GLOBAL_NOTHROW_T.cpp checks if operator new(size_t, ::nothrow_t)
 * is defined. In each of the tests that follow, we define nothrow_t
 * in the namespace in which it resides in the support library, and
 * in the case that it was defined in the std namespace, we bring
 * it into the the global namespace with a using declaration.
 * This way we can use the same form for both cases.
 * [Note: We make the assumption that all of the nothrow versions
 * of the operators will use the same signature.]
 *
 **********************************************************************/

#include "config.h"

#include <stddef.h>   // for size_t

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


void* operator new (size_t, const nothrow_t&) throw ();

int main ()
{
    ::operator new (1, nothrow_t ());

    return 0;
}
