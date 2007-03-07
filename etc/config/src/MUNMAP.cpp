// checking for munmap() in <sys/mman.h>

#include "config.h"

// include <sys/types.h> first in case mman.h depends
// on it (such as the one on Darwin -- see STDCXX-107)
#include <sys/types.h>   // for size_t
#include <sys/mman.h>    // for munmap()

extern "C" {

typedef int (f_i_vpui)(void*, size_t);
typedef int (f_i_cpui)(char*, size_t);

}


const char* munmap_arg2_type_imp (f_i_vpui*) { return "void*"; }
const char* munmap_arg2_type_imp (f_i_cpui*) { return "char*"; }

// determine whether the type of the second argument
// to munmap() is void* or char*
const char* get_munmap_arg2_type ()
{
    return munmap_arg2_type_imp (&munmap);
}


// include last so as not to pollute the global namespace
// and skew the result of the test
#include <stdio.h>


int main ()
{
    const char *argtype = get_munmap_arg2_type ();

    printf ("#define _RWSTD_MUNMAP_ARG1_T %s\n", argtype);

    return 0;
}
