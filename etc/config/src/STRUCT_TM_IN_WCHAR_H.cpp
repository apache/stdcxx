// checking for struct tm declaration in <wchar.h>

#include <wchar.h>

// 7.24.1, p2 of C99: struct tm is required to declared
// as an incomplete type in <wchar.h>

tm* struct_tm_declared (tm* arg)
{
    return arg;
}
