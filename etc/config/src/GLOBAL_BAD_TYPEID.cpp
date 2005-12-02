// checking for class ::bad_typeid

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#ifndef _RWSTD_NO_TYPEINFO
#  include <typeinfo>
#else   // if defined (_RWSTD_NO_TYPEINFO)
#  ifndef _RWSTD_NO_TYPEINFO_H
#    include <typeinfo.h>
#  else   // if defined (_RWSTD_NO_TYPEINFO_H)

// 5.2.8, p6: using typeid() without #including <typeinfo>
//            first yields undefined behavior...

class bad_typeid { };

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


struct S { virtual ~S () { } };

int main ()
{
    try {
        S *s = 0;

        // expected to throw [std::] bad_typeid
        typeid (*s);
    }
    catch (bad_typeid&) {
        return 0;
    }
    catch (...) {
        // fail if typeid throws anything other than a std::bad_typeid
        return 1;
    }

    // fail if typeid doesn't throw
    return 2;
}
