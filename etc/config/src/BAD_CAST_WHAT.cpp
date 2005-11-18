// checking for bad_cast::what()

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#define TEST_WHAT
#define bad_alloc bad_cast
#define main      test_bad_cast_what
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_bad_cast_what (argc, argv);
}
