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

# _AC_LANG_LCONV_STRUCT
# ---------------------
# Expands into program source code that outputs the 'lconv' structure.
AC_DEFUN([_AC_LANG_LCONV_STRUCT],
[_AC_LANG_DISPATCH([$0], _AC_LANG, $@)])

m4_define([_AC_LANG_LCONV_STRUCT(C++)],
[AC_LANG_PROGRAM([

@%:@include <locale.h>   // for lconv
@%:@include <stddef.h>   // for offsetof, size_t
@%:@include <stdio.h>    // for fprintf
@%:@include <string.h>   // for strlen

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

  fprintf (f, "{   /* sizeof (struct lconv) == %u */ \\\",
           sizeof (struct lconv));
  fputc ('\n', f);

  struct {
    size_t      offset;
    size_t      size;
    const char *type;
    const char *name;
    const char *comment;
  } info[[]] = {

#define ENTRY(type, member, comment)   \
    {                                  \
        OFF (lconv, member),           \
        sizeof ((lconv*)0)->member,    \
        type,                          \
        # member,                      \
        "/* " comment " */"            \
    }

    // C90 and POSIX.1 members
    ENTRY ("char*", decimal_point, ""),
    ENTRY ("char*", thousands_sep, ""),
    ENTRY ("char*", grouping, ""),
    ENTRY ("char*", int_curr_symbol, ""),
    ENTRY ("char*", currency_symbol, ""),
    ENTRY ("char*", mon_decimal_point, ""),
    ENTRY ("char*", mon_thousands_sep, ""),
    ENTRY ("char*", mon_grouping, ""),
    ENTRY ("char*", positive_sign, ""),
    ENTRY ("char*", negative_sign, ""),
    ENTRY ("char", int_frac_digits, ""),
    ENTRY ("char", frac_digits, ""),
    ENTRY ("char", p_cs_precedes, ""),
    ENTRY ("char", p_sep_by_space, ""),
    ENTRY ("char", n_cs_precedes, ""),
    ENTRY ("char", n_sep_by_space, ""),
    ENTRY ("char", p_sign_posn, ""),
    ENTRY ("char", n_sign_posn, "")

#ifndef _RWSTD_NO_LCONV_INT_FMAT

    // C99 and POSIX.2 members
    , ENTRY ("char", int_p_cs_precedes, ""),
    ENTRY ("char", int_n_cs_precedes, ""),
    ENTRY ("char", int_p_sep_by_space, ""),
    ENTRY ("char", int_n_sep_by_space, ""),
    ENTRY ("char", int_p_sign_posn, ""),
    ENTRY ("char", int_n_sign_posn, "")

#endif   // _RWSTD_NO_LCONV_INT_FMAT

  };

  const size_t N = sizeof info / sizeof *info;

  size_t curoff = 0;   // current offset
  size_t size   = 0;   // cumulative size

  for (size_t i = 0; i != N; ++i) {
    for (size_t j = 0; j != N; ++j) {
      if (curoff == info [[j]].offset) {

        fprintf (f, "    %s %s;  \\\",
                 info [[j]].type, info [[j]].name);
        fputc ('\n', f);

        // invalidate offset
        info [[j]].offset = size_t (-1);
        curoff += info [[j]].size;
        size   += info [[j]].size;

        break;
      }
    }
  }

  if (size != sizeof (lconv)) {
    fprintf (f, "    char _C_fill [[%u]];   /* fill to size */ \\\",
             sizeof (lconv) - size);
    fputc ('\n', f);
  }

  fputc ('}', f);

  return ferror (f) || fclose (f) != 0;
])])


# AC_STRUCT_LCONV_DEF(VARIABLE)
# ----------------------
# Define VARIABLE as the 'lconv' structure in VARIABLE.
#
AC_DEFUN([AC_STRUCT_LCONV_DEF],
[
  AC_PREREQ(2.62)dnl

  AC_CACHE_CHECK([definition of struct lconv],
                 AS_TR_SH([ac_cv_struct_lconv_def]),
  [
    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

    _AC_RUN_IFELSE([_AC_LANG_LCONV_STRUCT],
      [ac_cv_struct_lconv_def=`cat conftest.val`
       rm conftest.val],
      [AC_MSG_FAILURE([cannot determine definition of 'lconv' structure, 77])])

    AC_LANG_RESTORE
  ])dnl

  AC_DEFINE_UNQUOTED(AS_TR_CPP($1), [$ac_cv_struct_lconv_def],
                     [Definition of the `lconv' structure.])
])# AC_STRUCT_LCONV_DEF

