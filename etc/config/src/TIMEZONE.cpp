// checking for int timezone in <time.h>

#include <time.h>

int get_timezone_value ()
{
    // POSIX requires a timezone variable:
    // http://www.opengroup.org/onlinepubs/009695399/functions/timezone.html

    // BSD-based systems such as Darwin define a char* timezone(int, int)
    // function that has nothing to do with the variable:
    // http://developer.apple.com/documentation/Darwin/Reference/ManPages/ \
    // man3/timezone.3.html

    // check to see if a timezone variable exists and is convertible to int
    return timezone / 2;
}
