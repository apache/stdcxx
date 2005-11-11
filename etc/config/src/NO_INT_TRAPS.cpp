// checking if integer math traps

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

int get_int ();

int main (int argc, char*[])
{
    int int_zero = get_int ();
    int int_one  = get_int ();

    int result = int_one / int_zero;

    // NEGATIVE test: successful exit status indicates a failure
    return argc < 2 ? 0 : result;
}

// foil optimizers
volatile int int_value = 0;

int get_int ()
{
    return int_value++;
}
