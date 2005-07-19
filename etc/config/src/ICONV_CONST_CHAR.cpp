// checking for POSIX iconv()

#include <iconv.h>

int foo ()
{
    const char* const_ptr = 0;
    char* non_const_ptr   = 0;

    iconv_t *it = 0;

    return iconv (*it, &const_ptr, 0, &non_const_ptr, 0);
}
