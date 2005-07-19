// checking for munmap() in <sys/mman.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#include <sys/mman.h>

extern "C" {

typedef int (f_i_vpui)(void*, size_t);
typedef int (f_i_cpui)(char*, size_t);

}


const char* foo (f_i_vpui*) { return "void*"; }
const char* foo (f_i_cpui*) { return "char*"; }


const char* bar ()
{
    return foo (&munmap);
}


// include last so as not to pollute the global namespace
// and skew the result of the test
#include <stdio.h>


int main ()
{
    const char *argtype = bar ();

#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_MUNMAP\n");

#endif   // _RWSTD_USE_CONFIG
    
    printf ("#define _RWSTD_MUNMAP_ARG1_T %s\n", argtype);

    return 0;
}
