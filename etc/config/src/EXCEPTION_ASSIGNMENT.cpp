// checking for exception assignment operator

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif

#define TEST_ASSIGNMENT
#define bad_alloc exception
#define main      test_exception_assignment
#include "BAD_ALLOC_ASSIGNMENT.cpp"
#undef main

int main (int argc, char *argv[])
{
    return test_exception_assignment (argc, argv);
}
