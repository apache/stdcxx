// checking for implicit file inclusion

#include "implicit_inclusion_imp.h"

int main ()
{
    S<int> si;

    int result = si.foo ();

#ifndef _RWSTD_NO_MEMBER_TEMPLATES

    // exercise implicit inclusion with member templates
    // to detect SunPro 5.7 bug #475

    S<double> sd;

    result += sd.bar (result);

#endif   // _RWSTD_NO_MEMBER_TEMPLATES

    return result;
}
