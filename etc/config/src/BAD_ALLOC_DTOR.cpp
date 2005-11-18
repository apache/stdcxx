// checking for bad_alloc dtor

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#define TEST_DTOR
#define main test_bad_alloc_dtor
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_bad_alloc_dtor (argc, argv);
}
