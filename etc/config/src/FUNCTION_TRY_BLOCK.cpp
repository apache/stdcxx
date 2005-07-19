// checking function-try-block

#if defined _RWSTD_USE_CONFIG
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

int tried;
int caught;

template <class T>
T foo (T t)
    try {   // <-- testing
        ++tried;
        throw t;
    }
    catch (T) {
        ++caught;
        throw;
    }


struct S
{
    S (int i)
        try   // <-- testing
        : i_ (i) {
            ++tried;
            foo (i_);
        }
        catch (int) {
            ++caught;
            throw;
        }

    int i_;
};


int main ()
{
    int result = 0;

    try {
        S s (123);

        (void)&s;

        result |= 1;
    }
    catch (int i) {
        if (i != 123)
            result |= 2;

        if (tried != 2)
            result |= 4;

        if (caught != 2)
            result |= 8;
    }
    catch (...) {
        result |= 0x10;
    }

    return result;
}
