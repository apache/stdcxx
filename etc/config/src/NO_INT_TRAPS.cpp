// checking if integer math traps

#include "config.h"


#if (defined (_WIN32) || defined (_WIN64)) && !defined (__CYGWIN__)
#  define TRY             __try
#  define EXCEPT(expr)    __except (expr)
#else   // not Windows
#  define TRY               if (1)
#  define EXCEPT(ignore)    else if (0)
#endif   // _WIN{32,64}


int get_int ();

int main (int argc, char*[])
{
    int int_zero = get_int ();
    int int_one  = get_int ();

    int result;

    TRY {
        result = int_one / int_zero;
    }
    EXCEPT (1) {
        return 1;
    }

    // NEGATIVE test: successful exit status indicates a failure
    return argc < 2 ? 0 : result;
}

// foil optimizers
volatile int int_value = 0;

int get_int ()
{
    return int_value++;
}
