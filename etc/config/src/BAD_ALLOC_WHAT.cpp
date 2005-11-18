// checking for bad_alloc::what()

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#define TEST_WHAT
#define main test_bad_alloc_what
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_bad_alloc_what (argc, argv);
}
