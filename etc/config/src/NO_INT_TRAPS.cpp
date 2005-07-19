// checking if integer math traps

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

int int_get_zero ();

int main ()
{
    int int_zero = int_get_zero ();
    int int_one  = int_get_zero () + 1;
    int int_two  = int_get_zero () + 2;

    int x = int_one;

    while (int_zero < x)
        x *= int_two;

    // NEGATIVE test: successful exit status indicates a failure
    return 0;
}

// foil optimizers
int int_get_zero ()
{
    static int value = 0;

    return value;
}
