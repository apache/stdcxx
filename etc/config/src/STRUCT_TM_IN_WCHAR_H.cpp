// checking for struct tm in <wchar.h>

#include <wchar.h>

struct tm foo (struct tm arg)
{
    return arg;
}
