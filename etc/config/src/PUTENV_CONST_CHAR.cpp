// checking for putenv() in <stdlib.h>

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
