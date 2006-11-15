
#include "config.h"

#if defined (_WIN32) && defined (_DLL)
__declspec (dllexport)
#endif  // _WIN32 && _DLL
int foo (int i)
{
    if (i)
        throw i;

    return foo (i + 1);
}
