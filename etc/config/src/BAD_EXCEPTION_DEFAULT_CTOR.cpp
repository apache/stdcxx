// checking for bad_exception default ctor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#define TEST_DEFAULT_CTOR
#define bad_alloc bad_exception
#define main      test_bad_exception_default_ctor
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_bad_exception_default_ctor (argc, argv);
}
