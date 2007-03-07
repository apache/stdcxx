// checking for operator delete[] (void*)

#include "config.h"

#include <stddef.h>


#ifdef _RWSTD_NO_EXCEPTION_SPECIFICATION
#  define throw()
#endif   // _RWSTD_NO_EXCEPTION_SPECIFICATION

void operator delete[] (void*) throw ();

int main ()
{
    void* ptr = 0;
    operator delete[] (ptr);
    return 0;
}
