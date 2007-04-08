// checking for daylight variable in <time.h>

#include <time.h>

// check to see if the XSI extension to POSIX, daylight,
// is declared in the header <time.h>

int check_daylight ()
{
    return daylight;
}
