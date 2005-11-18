// checking for exception copy ctor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#define TEST_COPY_CTOR
#define bad_alloc exception
#define main      test_exception_copy_ctor
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_exception_copy_ctor (argc, argv);
}
