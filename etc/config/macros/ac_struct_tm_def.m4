#
# Licensed to the Apache Software  Foundation (ASF) under one or more
# contributor  license agreements.  See  the NOTICE  file distributed
# with  this  work  for  additional information  regarding  copyright
# ownership.   The ASF  licenses this  file to  you under  the Apache
# License, Version  2.0 (the  License); you may  not use  this file
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
# Copyright 1999-2008 Rogue Wave Software, Inc.
#
 
# _AC_LANG_TM_STRUCT
# -----------------
# Expands into program source code that outputs the 'tm' structure.
AC_DEFUN([_AC_LANG_TM_STRUCT],
[_AC_LANG_DISPATCH([$0], _AC_LANG, $@)])

m4_define([_AC_LANG_TM_STRUCT(C++)],
[AC_LANG_PROGRAM([
@%:@include <stddef.h> // offsetof, size_t
@%:@include <stdio.h> // file I/O
@%:@include <string.h> // strlen
@%:@include <time.h> // 'tm' structure

#if defined HAVE_OFFSETOF
#  define OFF(T, m)   offsetof (T, m)
#else
   // provide own equivalent when offsetof() macro doesn't work
   // (e.g., when using the EDG eccp front end with an incompatible
   // version of gcc)
#  define OFF(T, m)   (((const char*)&((T*)0)->m) - ((const char*)(T*)0))
#endif // !defined HAVE_OFFSETOF

// prevent IBM xlC 5.0 errors caused by using /usr/include/string.h
// which #defines these (and other) names to undeclared symbols
#undef strlen],
[
  FILE* f = fopen ("conftest.val", "w");
  if (!f) return 1;

  fprintf (f, "{   /* sizeof (struct tm) == %u */ \\\",
           sizeof (struct tm));
  fputc ('\n', f);

  struct {
    size_t      offset;
    const char *name;
    const char *comment;
  } info[[]] = {

#define ENTRY(member, comment) \
        { OFF (struct tm, member), # member ";", "   /* " comment " */" }

    ENTRY (tm_sec,   "seconds after the minute [[O..61]] "),
    ENTRY (tm_min,   "minutes after the hour   [[0..59]] "),
    ENTRY (tm_hour,  "hours since midnight     [[0..23]] "),
    ENTRY (tm_mday,  "day of the month         [[1..31]] "),
    ENTRY (tm_mon,   "months since January     [[0..11]] "),
    ENTRY (tm_year,  "years since 1900                 "),
    ENTRY (tm_wday,  "days since Sunday        [[0..6]]  "),
    ENTRY (tm_yday,  "days since January 1     [[0..365]]"),
    ENTRY (tm_isdst, "Daylight Saving Time             "),
  };

  const size_t N = sizeof info / sizeof *info;
  size_t curoff = 0;   // current offset

  // assume contiguous standard members
  // optionally followed by non-standard ones
  for (size_t i = 0; i != N; ++i) {
    for (size_t j = 0; j != N; ++j) {
      if (curoff == info [[j]].offset) {
        fprintf (f, "    int %-10s        %s \\\",
                 info [[j]].name, info [[j]].comment);
        fputc ('\n', f);

        info [[j]].offset = size_t (-1);
        curoff += sizeof (int);
        break;
      }
    }

    if (!curoff) {
      curoff = sizeof (struct tm) - N * sizeof (int);
    }
  }

  if (N * sizeof (int) != sizeof (struct tm)) {
    fprintf (f, "    char _C_fill [[%u]];   /* fill to size */ \\\",
             sizeof (struct tm) - N * sizeof (int));
    fputc ('\n', f);
  }

  fputc ('}', f);

  return ferror (f) || fclose (f) != 0;
])])


# AC_STRUCT_TM_DEF(VARIABLE)
# ----------------------
# Define VARIABLE as the 'tm' structure from <time.h> in VARIABLE.
#
AC_DEFUN([AC_STRUCT_TM_DEF],
[
  AC_PREREQ(2.62)dnl

  AC_CACHE_CHECK([definition of struct tm],
                 AS_TR_SH([ac_cv_struct_tm_def]),
  [AC_LANG_SAVE
  AC_LANG_CPLUSPLUS

  _AC_RUN_IFELSE([_AC_LANG_TM_STRUCT],
      [ac_cv_struct_tm_def=`cat conftest.val`
       rm conftest.val],
      [AC_MSG_FAILURE([cannot determine definition of 'tm' structure, 77])])

  AC_LANG_RESTORE
  ])dnl

  AC_DEFINE_UNQUOTED(AS_TR_CPP($1), [$ac_cv_struct_tm_def],
                     [Definition of the `tm' structure.])
])# AC_STRUCT_TM_DEF

