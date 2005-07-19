// checking for C++-style C library headers

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cfloat>
#include <climits>
#include <clocale>
#include <cmath>
#include <csetjmp>
#include <csignal>
#include <cstdarg>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "config.h"

#ifndef _RWSTD_NO_WCHAR_H
#  include <cwchar>
#endif   // _RWSTD_NO_WCHAR_H

#ifndef _RWSTD_NO_WCTYPE_H
#  include <cwctype>
#endif   // _RWSTD_NO_WCTYPE_H


int main ()
{
    return 0;
}
