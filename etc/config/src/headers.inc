##############################################################################
#
# Licensed to the Apache Software  Foundation (ASF) under one or more
# contributor  license agreements.  See  the NOTICE  file distributed
# with  this  work  for  additional information  regarding  copyright
# ownership.   The ASF  licenses this  file to  you under  the Apache
# License, Version  2.0 (the  "License"); you may  not use  this file
# except in  compliance with the License.   You may obtain  a copy of
# the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the  License is distributed on an  "AS IS" BASIS,
# WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
# implied.   See  the License  for  the  specific language  governing
# permissions and limitations under the License.
#
# Copyright 2001-2007 Rogue Wave Software, Inc.
#
##############################################################################

# list of headers
hdrs="assert complex ctype errno fenv float inttypes iso646 limits locale    \
      math setjmp signal stdarg stdbool stddef stdio stdint stdlib string    \
      tgmath time uchar wchar wctype new typeinfo"

hdrs="$hdrs ieeefp.h pthread.h"


# math functions
c90_funs="acos(0.0) asin(0.0) atan(0.0) atan2(0.0,0.0) ceil(0.0) cos(0.0)    \
          cosh(0.0) exp(0.0) fabs(0.0) floor(0.0) fmod(0.0,0.0) frexp(0.0,0) \
          ldexp(0.0,0) log(0.0) log10(0.0) modf(0.0,0) pow(0.0,0.0) sin(0.0) \
          sinh(0.0) sqrt(0.0) tan(0.0) tanh(0.0) acosf asinf atanf atan2f    \
          ceilf cosf coshf expf fabsf floorf fmodf frexpf ldexpf logf log10f \
          modff powf sinf sinhf sqrtf tanf tanhf acosl asinl atanl atan2l    \
          ceill cosl coshl expl fabsl floorl fmodl frexpl ldexpl logl log10l \
          modfl powl sinl sinhl sqrtl tanl tanhl"
c99_funs="cbrtf copysignf erfcf erff expm1f exp2f fdimf fmaf fmaxf fminf     \
          hypotf ilogbf lgammaf logbf log1pf log2f llrintf lrintf lroundf    \
          llroundf nanf nearbyintf nextafterf nexttowardf remainderf remquof \
          rintf roundf scalbnf scalblnf tgammaf cbrt copysign erf erfc expm1 \
          exp2 fdim fma fmax fmin hypot ilogb lgamma logb log1p log2 llrint  \
          lrint lround llround nan nearbyint nextafter nexttoward remainder  \
          remquo rint round scalbn scalbln tgamma cbrtl copysignl erfcl erfl \
          expm1l exp2l fdiml fmal fmaxl fminl hypotl ilogbl lgammal logbl    \
          log1pl log2l llrintl lrintl lroundl llroundl nanl nearbyintl       \
          nextafterl nexttowardl remainderl remquol rintl roundl scalbnl     \
          scalblnl tgammal"

# ignore C99 functions for now
math="$c90_funs"


# stdio functions
c90_funs="clearerr fclose feof ferror fflush fgetc fgetpos fgets fopen       \
          fprintf fputc fputs fread freopen fscanf fseek fsetpos ftell       \
          fwrite getc getchar gets perror printf putc putchar puts remove    \
          rename rewind scanf setbuf setvbuf sprintf sscanf tmpfile tmpnam   \
          ungetc vfprintf vprintf vsprintf"
c99_funs="snprintf vsnprintf"
posix_funs="fileno"

stdio="$c90_funs $c99_funs $posix_funs"


# stdlib functions
c90_funs="abort abs atexit atof atoi atol atoll bsearch calloc div exit free \
          getenv labs ldiv llabs lldiv malloc mblen mbstowcs mbtowc qsort    \
          rand realloc srand strtod strtol strtoul system wcstombs wctomb"
c99_funs="strtof strtold strtoll strtoull"
posix_funs="mkstemp putenv setenv unsetenv"

stdlib="$c90_funs $c99_funs $posix_funs"


# string functions
string="memchr((void*)0,0,0) memcmp memcpy memmove memset strcat             \
        strchr((char*)0,0) strcmp strcoll strcpy strcspn strerror strlen     \
        strncat strncmp strncpy strpbrk((char*)0,(char*)0)                   \
        strrchr((char*)0,0) strspn strstr((char*)0,(char*)0) strtok strxfrm"


# time functions
c90_funs="asctime clock ctime difftime gmtime localtime mktime strftime time"
c99_funs="mkxtime strfxtime zonetime"
posix_funs="tzset"

# ignore C99 functions for now
time="$c90_funs $posix_funs"


# wchar functions
wchar="btowc fgetwc fgetws fputwc fputws fwide fwprintf fwscanf getwc        \
       getwchar mbrlen mbrtowc mbsinit mbsrtowcs putwc putwchar swprintf     \
       swscanf ungetwc vfwprintf vswprintf vwprintf vwscanf wcrtomb wcscat   \
       wcschr((wchar_t*)0,0) wcscmp wcscoll wcscpy wcscspn wcsftime wcslen   \
       wcsncat wcsncmp wcsncpy wcspbrk((wchar_t*)0,(wchar_t*)0)              \
       wcsrchr((wchar_t*)0,0) wcsrtombs wcsspn                               \
       wcsstr((wchar_t*)0,(wchar_t*)0) wcstod wcstof wcstok wcstol wcstold   \
       wcstoll wcstoul wcstoull wcsxfrm wctob wmemchr((wchar_t*)0,0,0)       \
       wmemcmp wmemcpy wmemmove wmemset wprintf wscanf"


# wctype functions
# 7.15 of ISO/IEC 9899:1990/Amendment 1:1995
wctype="iswalpha iswalnum iswcntrl iswdigit iswgraph iswlower iswprint       \
        iswpunct iswspace iswupper iswxdigit wctype iswctype towlower        \
        towupper wctrans towctrans"
