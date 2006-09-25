// checking for thread-local storage

#include <stdio.h>

#if defined (_MSC_VER)
#  define _RWSTD_THREAD __declspec (thread)
#else
#  define _RWSTD_THREAD __thread
#endif

#define STR(x) #x
#define PRINT_MACRO(name) printf ("#define " #name " " STR (name) "\n")

_RWSTD_THREAD int tls_i;

static _RWSTD_THREAD char tls_static_array [256];
extern _RWSTD_THREAD char tls_extern_array [256];

_RWSTD_THREAD char tls_extern_array [256];

int main ()
{
    int result = tls_i + tls_static_array [0] + tls_extern_array [0];

    if (0 == result)
        PRINT_MACRO (_RWSTD_THREAD);

    return result;
}
