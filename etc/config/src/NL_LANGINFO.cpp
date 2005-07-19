// checking for nl_langinfo() in <langinfo.h>

#include <langinfo.h>

// compile and link to make sure nl_langinfo() is not only
// declared, but also defined in one of the libraries we
// link with
int main (int argc, char *argv[])
{
    const char *d_t_fmt = nl_langinfo (D_T_FMT);

    if (argc > 1)
        return 0 != d_t_fmt;

    return 0;
}
