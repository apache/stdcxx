// checking for exception dtor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#ifdef _RWSTD_NO_EXCEPTION_DEFAULT_CTOR
#  define NO_DEFAULT_CTOR
#endif

#define bad_alloc exception
#define main      test_exception_dtor
#include "BAD_ALLOC_DTOR.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_exception_dtor (argc, argv);
}
