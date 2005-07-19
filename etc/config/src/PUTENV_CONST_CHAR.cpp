// checking for putenv() in <stdlib.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#include <stdlib.h>

// [re]declare with the expected POSIX signature which will fail
// to compile if the implementation declares putenv(char*)
// (e.g., HP-UX, IRIX, etc.)
extern "C" int putenv (const char*);

int foo ()
{
    const char *var = "foobar";

    return putenv (var);
}
