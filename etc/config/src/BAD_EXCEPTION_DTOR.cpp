// checking for bad_exception dtor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#define TEST_DTOR
#define bad_alloc bad_exception
#define main      test_bad_exception_dtor
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_bad_exception_dtor (argc, argv);
}
