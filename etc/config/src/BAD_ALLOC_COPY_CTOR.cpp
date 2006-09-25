// checking for bad_alloc copy ctor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#if 0   // guard invalid preprocessor symbol below
   // establish a dependency on RUNTIME_IN_STD.cpp
#  ifndef _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_RUNTIME_IN_STD
#endif   // 0

#define TEST_COPY_CTOR
#define main      test_bad_alloc_copy_ctor
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_bad_alloc_copy_ctor (argc, argv);
}
