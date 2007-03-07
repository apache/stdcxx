// checking for class std::bad_typeid

#include "config.h"

#ifndef _RWSTD_NO_TYPEINFO
#  include <typeinfo>
#else
#  ifndef _RWSTD_NO_TYPEINFO_H
#    include <typeinfo.h>
#  else

// 5.2.8, p6 [expr.typeid]:
// If the header <typeinfo> is not included prior to a use of typeid,
// the program is ill-formed.
namespace std {

class type_info { };

class bad_typeid { };

}   // namespace std

#  endif   // _RWSTD_NO_TYPEINFO_H
#endif   // _RWSTD_NO_TYPEINFO


#if 2 == __GNUG__
#  ifndef _RWSTD_NO_HONOR_STD
#    ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    static int *ip;

    if ((ip [0] = ip [1])) {  // force a SIGSEGV
        ++ip;
        terminate ();         // recurse infinitely
    }
}

}   // namespace std

#    endif   // _RWSTD_NO_STD_TERMINATE
#  endif   // _RWSTD_NO_HONOR_STD
#endif   // gcc 2.x


// force a failure if namespace std isn't honored
enum bad_typeid { };


struct S { virtual ~S () { } };

int main ()
{
    try {
        S *s = 0;

        // throws [std::] bad_typeid
        typeid (*s);
    }
    catch (std::bad_typeid&) {
        return 0;
    }
    catch (...) {
        // fail if typeid throws anything other than a std::bad_typeid
        return 1;
    }

    // fail if typeid doesn't throw
    return 2;
}
