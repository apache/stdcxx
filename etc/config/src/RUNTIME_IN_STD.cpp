// checking for runtime support in namespace std

#if defined (_RWSTD_USE_CONFIG)
#  include "config.h"
#endif   // _RWSTD_USE_CONFIG

// for debugging convenience
enum {
    STD_BAD_ALLOC = 1,
    STD_BAD_CAST = 1,
    STD_BAD_EXCEPTION = 1,
    STD_BAD_TYPEID = 1,
    STD_EXCEPTION = 1,
    STD_SET_NEW_HANDLER = 1,
    STD_SET_TERMINATE = 1,
    STD_SET_UNEXPECTED = 1,
    STD_TERMINATE = 1,
    STD_UNCAUGHT_EXCEPTION = 1,
    STD_UNEXPECTED = 1
};


int std_defs = 0

#ifndef _RWSTD_NO_HONOR_STD

   // avoid false positives if namespace std is not honored

#  ifndef _RWSTD_NO_STD_BAD_ALLOC
    + STD_BAD_ALLOC
#  endif   // _RWSTD_NO_STD_BAD_ALLOC

#  ifndef _RWSTD_NO_STD_BAD_CAST
    + STD_BAD_CAST
#  endif   // _RWSTD_NO_STD_BAD_CAST

#  ifndef _RWSTD_NO_STD_BAD_EXCEPTION
    + STD_BAD_EXCEPTION
#  endif   // _RWSTD_NO_STD_BAD_EXCEPTION

#  ifndef _RWSTD_NO_STD_BAD_TYPEID
    + STD_BAD_TYPEID
#  endif   // _RWSTD_NO_STD_BAD_TYPEID

#  ifndef _RWSTD_NO_STD_EXCEPTION
    + STD_EXCEPTION
#  endif   // _RWSTD_NO_STD_EXCEPTION

#  ifndef _RWSTD_NO_STD_SET_NEW_HANDLER
    + STD_SET_NEW_HANDLER
#  endif   // _RWSTD_NO_STD_SET_NEW_HANDLER

#  ifndef _RWSTD_NO_STD_SET_TERMINATE
    + STD_SET_TERMINATE
#  endif   // _RWSTD_NO_STD_SET_TERMINATE

#  ifndef _RWSTD_NO_STD_SET_UNEXPECTED
    + STD_SET_UNEXPECTED
#  endif   // _RWSTD_NO_STD_SET_UNEXPECTED

#  ifndef _RWSTD_NO_STD_TERMINATE
    + STD_TERMINATE
#  endif   // _RWSTD_NO_STD_TERMINATE

#  ifndef _RWSTD_NO_STD_UNCAUGHT_EXCEPTION
    + STD_UNCAUGHT_EXCEPTION
#  endif   // _RWSTD_NO_STD_UNCAUGHT_EXCEPTION

#  ifndef _RWSTD_NO_STD_UNEXPECTED
    + STD_UNEXPECTED
#  endif   // _RWSTD_NO_STD_UNEXPECTED
#endif   // _RWSDT_NO_HONOR_STD
    + 0;


// for debugging convenience
enum {
    GLOBAL_BAD_ALLOC = 1,
    GLOBAL_BAD_CAST = 1,
    GLOBAL_BAD_EXCEPTION = 1,
    GLOBAL_BAD_TYPEID = 1,
    GLOBAL_EXCEPTION = 1,
    GLOBAL_SET_NEW_HANDLER = 1,
    GLOBAL_SET_TERMINATE = 1,
    GLOBAL_SET_UNEXPECTED = 1,
    GLOBAL_TERMINATE = 1,
    GLOBAL_UNCAUGHT_EXCEPTION = 1,
    GLOBAL_UNEXPECTED = 1
};


int global_defs = 0
#ifndef _RWSTD_NO_GLOBAL_BAD_ALLOC
    + GLOBAL_BAD_ALLOC
#endif   // _RWSTD_NO_GLOBAL_BAD_ALLOC

#ifndef _RWSTD_NO_GLOBAL_BAD_CAST
    + GLOBAL_BAD_CAST
#endif   // _RWSTD_NO_GLOBAL_BAD_CAST

#ifndef _RWSTD_NO_GLOBAL_BAD_EXCEPTION
    + GLOBAL_BAD_EXCEPTION
#endif   // _RWSTD_NO_GLOBAL_BAD_EXCEPTION

#ifndef _RWSTD_NO_GLOBAL_BAD_TYPEID
    + GLOBAL_BAD_TYPEID
#endif   // _RWSTD_NO_GLOBAL_BAD_TYPEID

#ifndef _RWSTD_NO_GLOBAL_EXCEPTION
    + GLOBAL_EXCEPTION
#endif   // _RWSTD_NO_GLOBAL_EXCEPTION

#ifndef _RWSTD_NO_GLOBAL_SET_NEW_HANDLER
    + GLOBAL_SET_NEW_HANDLER
#endif   // _RWSTD_NO_GLOBAL_SET_NEW_HANDLER

#ifndef _RWSTD_NO_GLOBAL_SET_TERMINATE
    + GLOBAL_SET_TERMINATE
#endif   // _RWSTD_NO_GLOBAL_SET_TERMINATE

#ifndef _RWSTD_NO_GLOBAL_SET_UNEXPECTED
    + GLOBAL_SET_UNEXPECTED
#endif   // _RWSTD_NO_GLOBAL_SET_UNEXPECTED

#ifndef _RWSTD_NO_GLOBAL_TERMINATE
    + GLOBAL_TERMINATE
#endif   // _RWSTD_NO_GLOBAL_TERMINATE

#ifndef _RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION
    + GLOBAL_UNCAUGHT_EXCEPTION
#endif   // _RWSTD_NO_GLOBAL_UNCAUGHT_EXCEPTION

#ifndef _RWSTD_NO_GLOBAL_UNEXPECTED
    + GLOBAL_UNEXPECTED
#endif   // _RWSTD_NO_GLOBAL_UNEXPECTED
    + 0;

int main ()
{
    // since some (possibly many) individual config tests fail
    // to detect either an STD or a GLOBAL version of the feature,
    // this test #defines a convenience macro based on the overall
    // findings to avoid complex #ifdef logic in the library

    // the test succeeds (exits with a 0 exit status) with if at
    // least one symbol is found to exist in the C++ runtime library
    return !std_defs;
}
