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
 
# RW_STDC_STDIO
# -------------
# Performs all standard C library config checks for <stdio.h> header.
AC_DEFUN([RW_STDC_STDIO],
[
  AC_CHECK_HEADER([stdio.h],
  [
    AC_PATH_HEADER([_RWSTD_ANSI_C_STDIO_H], [stdio.h])

    AC_FOREACH([rw_Value],
        [EOF L_tmpnam _IOFBF _IOLBF _IONBF BUFSIZ FOPEN_MAX \
         FILENAME_MAX TMP_MAX],
        [_AC_CACHE_CHECK_INT([value of rw_Value],
                             [AS_TR_SH([rw_cv_stdio_]rw_Value)],
                             [(long int)(]rw_Value[)],
                             [@%:@include <stdio.h>])

         if test $AS_TR_SH([rw_cv_stdio_]rw_Value) -gt 0 ; then
           AC_DEFINE_UNQUOTED([_RWSTD_]rw_Value,
                              $AS_TR_SH([rw_cv_stdio_]rw_Value),
[Value of ]rw_Value[ symbol defined in <stdio.h> header.])
         fi
        ])

    RW_CHECK_FUNCS([clearerr fclose feof ferror fflush fgetc \
fgetpos fgets fopen fprintf fputc fputs fread freopen fscanf fseek \
fsetpos ftell fwrite getc getchar gets perror printf putc putchar puts \
remove rename rewind scanf setbuf setvbuf snprintf sprintf sscanf \
tmpfile tmpnam ungetc vfprintf vprintf vsnprintf vsprintf])
  ], [
    AC_DEFINE([_RWSTD_NO_STDIO_H], [1],
[Define to 1 if you don't have the <stdio.h> header file.])
  ])
])# RW_STDC_STDIO

