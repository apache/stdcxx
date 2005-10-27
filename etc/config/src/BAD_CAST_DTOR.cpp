// checking for bad_cast dtor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#ifdef _RWSTD_NO_BAD_CAST_DEFAULT_CTOR
#  define NO_DEFAULT_CTOR
#endif   // _RWSTD_NO_BAD_CAST_DEFAULT_CTOR

#define bad_alloc bad_cast
#define main      test_bad_cast_dtor
#include "BAD_ALLOC_DTOR.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_bad_cast_dtor (argc, argv);
}
