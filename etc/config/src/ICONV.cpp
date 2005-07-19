// checking for iconv() in <iconv.h>

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

#include <iconv.h>

extern "C" {

typedef size_t (f_i_cppc)(iconv_t, const char**, size_t*, char**, size_t*);
typedef size_t (f_i_cpp) (iconv_t,       char**, size_t*, char**, size_t*);

}


const char* foo (f_i_cppc*) { return "const char**"; }
const char* foo (f_i_cpp*)  { return "char**"; }


const char* bar ()
{
    return foo (&iconv);
}


// include last so as not to pollute the global namespace
// and skew the result of the test
#include <stdio.h>


int main ()
{
    const char *argtype = bar ();

#if !defined (_RWSTD_USE_CONFIG)

    printf ("/**/\n#undef _RWSTD_ICONV_ARG2_T\n");

#endif   // _RWSTD_USE_CONFIG

    if ('o' == argtype [1]) {
        // iconv() second argument type is const char**
        printf ("// ");
    }

    // may be commented out in the printf statement above
    printf ("#define _RWSTD_NO_ICONV_CONST_CHAR\n");

    printf ("#define _RWSTD_ICONV_ARG2_T %s\n", argtype);

    return 0;
}
