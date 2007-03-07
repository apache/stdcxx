// checking for bad_cast default ctor

#include "config.h"

#if 0   // guard invalid preprocessor symbol below
   // establish a dependency on RUNTIME_IN_STD.cpp
#  ifndef _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_RUNTIME_IN_STD
#endif   // 0

#define TEST_DEFAULT_CTOR
#define bad_alloc bad_cast
#define main      test_bad_cast_default_ctor
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_bad_cast_default_ctor (argc, argv);
}
