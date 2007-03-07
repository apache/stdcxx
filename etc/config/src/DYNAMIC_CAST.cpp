// checking for dynamic_cast

#include "config.h"

#if (defined (_WIN32) || defined (_WIN64)) && !defined (__CYGWIN__)
#  include <crtdbg.h>
#endif   // _WIN{32,64}


#ifndef _RWSTD_NO_HONOR_STD
#  ifdef _RWSTD_NO_STD_TERMINATE

namespace std {

void terminate ()
{
    for (char *p = 0; ; ++p) {
        *p = 0;   // force a SIGSEGV
    }
}

}

#  endif   // _RWSTD_NO_STD_TERMINATE
#endif   // _RWSTD_NO_HONOR_STD


struct A
{
    virtual ~A () { }
};

struct B: A { };
struct C: A { };


int main ()
{
#if (defined (_WIN32) || defined (_WIN64)) && !defined (__CYGWIN__)

    // prevent the Microsoft C library from popping up GUI windows
    _CrtSetReportMode (_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode (_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode (_CRT_ASSERT, _CRTDBG_MODE_DEBUG);

#endif   // _WIN{32,64}

    B b;

    A *a = &b;
    
    return !(dynamic_cast<B*>(a) && !dynamic_cast<C*>(a));
}
