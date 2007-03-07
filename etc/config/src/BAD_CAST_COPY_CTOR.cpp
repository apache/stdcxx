// checking for bad_cast copy ctor

#include "config.h"

#if 0   // guard invalid preprocessor symbol below
   // establish a dependency on RUNTIME_IN_STD.cpp
#  ifndef _RWSTD_NO_RUNTIME_IN_STD
#  endif   // _RWSTD_NO_RUNTIME_IN_STD
#endif   // 0

#define TEST_COPY_CTOR
#define bad_alloc bad_cast
#define main      test_bad_cast_copy_ctor
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_bad_cast_copy_ctor (argc, argv);
}
