// checking for catopen() in <nl_types.h>

#include <nl_types.h>
#include "config.h"

int foo ()
{
    nl_catd cat = catopen ("", 1);
    catgets (cat, 1, 2, "");
    catclose (cat);
    return 0;
}
