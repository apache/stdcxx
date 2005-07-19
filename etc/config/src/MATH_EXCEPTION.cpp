// checking for struct exception in <math.h>

#ifdef __osf__
   // preemptive strike in case _OSF_SOURCE is #defined
#  define _OSF_SOURCE
#endif   // __osf__


#include <math.h>

#undef exception

void foo ()
{
    // some systems (e.g., AIX) define struct exception in <math.h>
    // without guarding it for C++ to prevent potential collisions
    exception e;
    (void)&e;
}
