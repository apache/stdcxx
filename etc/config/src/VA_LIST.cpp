// checking the type of va_list

/***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  License); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 1999-2007 Rogue Wave Software, Inc.
 * 
 **************************************************************************/

#include "config.h"

#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>


// determine the underlying pointer type of a typedef
#define GET_TYPE_NAME(T)                                 \
    const char* get_type_name (T) { return # T; }        \
    const char* get_type_name (T*) { return # T "*"; }   \
    typedef void unused_type

GET_TYPE_NAME (char);
GET_TYPE_NAME (signed char);
GET_TYPE_NAME (unsigned char);
GET_TYPE_NAME (short);
GET_TYPE_NAME (unsigned short);
GET_TYPE_NAME (int);
GET_TYPE_NAME (unsigned int);
GET_TYPE_NAME (long);
GET_TYPE_NAME (unsigned long);

#ifndef _RWSTD_NO_LONG_LONG

GET_TYPE_NAME (long long);
GET_TYPE_NAME (unsigned long long);

#elif defined (_MSC_VER)

GET_TYPE_NAME (__int64);
GET_TYPE_NAME (unsigned __int64);

#endif   // _RWSTD_NO_LONG_LONG, _MSC_VER

GET_TYPE_NAME (float);
GET_TYPE_NAME (double);

#ifndef _RWSTD_NO_LONG_DOUBLE

GET_TYPE_NAME (long double);

#endif   // _RWSTD_NO_LONG_DOUBLE

GET_TYPE_NAME (void*);

const char* get_type_name (...) { return 0; }

#ifndef _RWSTD_NO_CLASS_PARTIAL_SPEC

template <class T>
struct Type {
    enum { array_size = 0 };
    static const char* type_name () { return get_type_name (T ()); }
};

template <class T, int N>
struct Type<T [N]> {
    enum { array_size = N };
    static const char* type_name () { return get_type_name (T ()); }
};


int va_list_array_size (va_list)
{
    return Type<va_list>::array_size;
}

const char* va_list_type_name (va_list)
{
    return Type<va_list>::type_name ();
}


#else   // if defined (_RWSTD_NO_CLASS_PARTIAL_SPEC)

#ifndef _RWSTD_NO_PART_SPEC_OVERLOAD

template <class T>
int va_list_array_size_imp (T *va)
{
    return 0;
}

template <class T>
int va_list_array_size_imp (T **va)
{
    return sizeof (va_list) / sizeof **va;
}

int va_list_array_size (va_list va)
{
    return va_list_array_size_imp (&va);
}

#else   // if defined (_RWSTD_NO_PART_SPEC_OVERLOAD)

template <class T>
class is_array
{ 
    class yes {};

    class no { yes yes_ [2]; };

    template <class U> struct Type {};

    template <class U, size_t N>
    static yes test (Type<U[N]>);
    static no test (...);

public:
    enum { value = sizeof (test (Type<T> ())) == sizeof (yes) };
};

template <class T>
int va_list_array_size_imp (T **va)
{
    return sizeof (va_list) / sizeof **va;
}

int va_list_array_size (va_list va)
{
    return is_array<va_list>::value ? va_list_array_size_imp (&va) : 0;
}

#endif  // _RWSTD_NO_PART_SPEC_OVERLOAD

template <class T>
const char* va_list_type (T *va)
{
    return get_type_name (*va);
}

const char* va_list_type_name (va_list va)
{
    va_list va2;
    memset (&va2, 0, sizeof va2);

    const int array_size = va_list_array_size (va2);

    return array_size ? va_list_type (va2) : get_type_name (va);
}

#endif   // _RWSTD_NO_CLASS_PARTIAL_SPEC


int main ()
{
#if defined (va_copy)

    // comment out the macro #defined below when va_copy() exists
    printf ("%s ", "//");

#endif   // va_copy

    printf ("#define _RWSTD_NO_VA_COPY\n");

    va_list va;
    memset (&va, 0, sizeof va);

    const int array_size = va_list_array_size (va);

    const char* type_name = va_list_type_name (va);

    if (0 == type_name) {
        puts ("typedef struct {");

        int elem_size = sizeof va;
        if (array_size)
            elem_size /= array_size;

        if (1 < elem_size / sizeof (void*))
            printf ("    void* _C_data [%u];\n", elem_size / sizeof (void*));
        else if (1 == elem_size / sizeof (void*))
            puts ("    void* _C_data;");
        else if (1 < elem_size % sizeof (void*))
            printf ("    char _C_data [%u]\n", elem_size % sizeof (void*));
        else if (1 == elem_size % sizeof (void*))
            puts ("    char _C_data;");

        puts ("} __rw_va_elem;");
        type_name = "__rw_va_elem";
    }

    if (array_size) {
        printf ("typedef %s __rw_va_list [%d];\n"
                "// #define _RWSTD_NO_VA_LIST_ARRAY   // va_list is an array\n",
                type_name, array_size);
    }
    else {
        printf ("typedef %s __rw_va_list;\n"
                "#define _RWSTD_NO_VA_LIST_ARRAY   // va_list is object type\n",
                type_name);
    }

    puts ("#define _RWSTD_VA_LIST __rw_va_list");

    return 0;
}
